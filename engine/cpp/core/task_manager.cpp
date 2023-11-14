
#include "core/task_manager.h"

namespace xpe {

    namespace core {

        void sTask::DoWork()
        {
            Todo();
        }

        void sTask::DoAll()
        {
            DoWork();
            sTask* nextTask = Next;
            while (nextTask != nullptr) {
                nextTask->DoWork();
                nextTask = nextTask->Next;
            }
        }

        cTaskDispatcher::cTaskDispatcher(u32 workerSize, usize taskBufferSize, const char* name, cThread::ePriority priority)
        {
            m_TasksTodo = 0;
            m_TasksDone.store(0);
            m_WorkerCount = workerSize;
            m_TaskBuffer.Resize(taskBufferSize);
            for (int i = 0; i < workerSize ; i++) {
                InitThread(i, name, priority);
            }
        }

        void cTaskDispatcher::Dispatch(sTask task)
        {
            m_TasksTodo += 1;
            // try to push a new task until it is pushed
            while (!m_TaskBuffer.Push(task)) {
                Poll();
            }
            m_WakeCondition.notify_one();
        }

        void cTaskDispatcher::Dispatch(u32 tasksPerThread, u32 taskSize, sTask task)
        {
            if (taskSize == 0 || tasksPerThread == 0) {
                return;
            }

            u32 taskGroups = (tasksPerThread + taskSize - 1) / taskSize;
            m_TasksTodo += taskGroups;

            for (u32 i = 0; i < taskGroups; ++i) {
                // create task group
                sTask taskGroup;
                taskGroup.Todo = [i, task, taskSize, tasksPerThread]() {
                    u32 groupJobOffset = i * taskSize;
                    u32 groupJobEnd = std::min(groupJobOffset + taskSize, tasksPerThread);
                    for (u32 j = groupJobOffset; j < groupJobEnd; ++j) {
                        ((sTask) task).DoAll();
                    }
                };

                // try to push new group as a task until its pushed
                while (!m_TaskBuffer.Push(taskGroup)) {
                    Poll();
                }

                m_WakeCondition.notify_one();
            }
        }

        bool cTaskDispatcher::IsBusy()
        {
            return m_TasksDone.load() < m_TasksTodo;
        }

        u32 cTaskDispatcher::GetWorkerCount() const
        {
            return m_WorkerCount;
        }

        void cTaskDispatcher::Wait()
        {
            while (IsBusy()) {
                Poll();
            }
        }

        void cTaskDispatcher::Poll()
        {
            m_WakeCondition.notify_one();
            std::this_thread::yield();
        }

        void cTaskDispatcher::InitThread(u32 workerId, const char* name, cThread::ePriority priority)
        {
            std::thread worker([this]() {
                sTask task;
                while (true) {
                    if (m_TaskBuffer.Pop(task)) {
                        // execute job and update worker label state
                        task.DoAll();
                        m_TasksDone.fetch_add(1);
                    } else {
                        // no job, put thread to sleep
                        std::unique_lock<std::mutex> lock(m_WakeMutex);
                        m_WakeCondition.wait(lock);
                    }
                }
            });
            cThread thread = { workerId, worker };
            thread.SetFormat(name, priority);
            worker.detach();
        }

        cSimulationDispatcher::~cSimulationDispatcher() = default;

        void cSimulationDispatcher::submitTask(physx::PxBaseTask& pxTask)
        {
            sTask task;
            task.Category = eTaskCategory::PHYSICS;
            task.Todo = [&pxTask]() {
                pxTask.run();
                pxTask.release();
            };
            Dispatch(task);
        }

        uint32_t cSimulationDispatcher::getWorkerCount() const
        {
            return GetWorkerCount();
        }

        cTaskDispatcher* cTaskManager::s_AudioDispatcher = nullptr;
        cTaskDispatcher* cTaskManager::s_NetworkDispatcher = nullptr;
        cSimulationDispatcher* cTaskManager::s_SimulationDispatcher = nullptr;
        cTaskDispatcher* cTaskManager::s_ThreadPoolDispatcher = nullptr;

        void cTaskManager::Init()
        {
            s_AudioDispatcher = new cTaskDispatcher(1, 10, "AudioThread", cThread::ePriority::NORMAL);
            s_NetworkDispatcher = new cTaskDispatcher(1, 10, "NetworkThread", cThread::ePriority::NORMAL);
            s_SimulationDispatcher = new cSimulationDispatcher(1, 10, "SimulationThread", cThread::ePriority::NORMAL);
            s_ThreadPoolDispatcher = new cTaskDispatcher(cHardwareManager::CPU.Cores - 3, 100, "ThreadPool", cThread::ePriority::NORMAL);
        }

        void cTaskManager::Free()
        {
            delete s_AudioDispatcher;
            delete s_NetworkDispatcher;
            delete s_SimulationDispatcher;
            delete s_ThreadPoolDispatcher;
        }

        void cTaskManager::SubmitTask(const sTask &task)
        {
            switch (task.Category)
            {
                case eTaskCategory::AUDIO:
                    s_AudioDispatcher->Dispatch(task);
                    break;
                case eTaskCategory::NETWORK:
                    s_NetworkDispatcher->Dispatch(task);
                    break;
                case eTaskCategory::PHYSICS:
                    s_SimulationDispatcher->Dispatch(task);
                    break;
                case eTaskCategory::ANIMATION:
                    s_SimulationDispatcher->Dispatch(task);
                    break;
                case eTaskCategory::THREAD_POOL:
                    s_ThreadPoolDispatcher->Dispatch(task);
                    break;
            }
        }

        void cTaskManager::SubmitTask(u32 tasksPerThread, u32 totalTasks, const sTask &task)
        {
            switch (task.Category)
            {
                case eTaskCategory::AUDIO:
                    s_AudioDispatcher->Dispatch(tasksPerThread, totalTasks, task);
                    break;
                case eTaskCategory::NETWORK:
                    s_NetworkDispatcher->Dispatch(tasksPerThread, totalTasks, task);
                    break;
                case eTaskCategory::PHYSICS:
                    s_SimulationDispatcher->Dispatch(tasksPerThread, totalTasks, task);
                    break;
                case eTaskCategory::ANIMATION:
                    s_SimulationDispatcher->Dispatch(tasksPerThread, totalTasks, task);
                    break;
                case eTaskCategory::THREAD_POOL:
                    s_ThreadPoolDispatcher->Dispatch(tasksPerThread, totalTasks, task);
                    break;
            }
        }

        cSimulationDispatcher *cTaskManager::GetSimulationDispatcher()
        {
            return s_SimulationDispatcher;
        }

    }

}
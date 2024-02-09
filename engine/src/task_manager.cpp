namespace xpe
{
    namespace core
    {
        sTaskQueue::sTaskQueue()
        {
            Todo = [this]()
            {
                for (auto& task : Tasks)
                {
                    task.Todo();
                }
            };
        }

        sTaskQueue::sTaskQueue(sTask::eType type, const vector<sTask> &tasks)
        {
            Type = type;
            Tasks = tasks;
            Todo = [this]()
            {
                for (auto& task : Tasks)
                {
                    task.Todo();
                }
            };
        }

        cTaskDispatcher::cTaskDispatcher(u32 workerSize, usize taskBufferSize, const char* name, cThread::ePriority priority)
        {
            m_TasksTodo = 0;
            m_TasksDone.store(0);
            m_WorkerCount = workerSize;
            m_TaskBuffer.Resize(taskBufferSize);
            for (int i = 0; i < workerSize ; i++)
            {
                InitThread(i, name, priority);
            }
        }

        void cTaskDispatcher::Dispatch(const sTask& task)
        {
            m_TasksTodo += 1;
            // try to push a new task until it is pushed
            while (!m_TaskBuffer.Push(task))
            {
                Poll();
            }
            m_WakeCondition.notify_one();
        }

        bool cTaskDispatcher::IsBusy()
        {
            return m_TasksDone.load() < m_TasksTodo;
        }

        u32 cTaskDispatcher::GetWorkerCount() const
        {
            return m_WorkerCount;
        }

        void cTaskDispatcher::Poll()
        {
            m_WakeCondition.notify_one();
            std::this_thread::yield();
        }

        void cTaskDispatcher::InitThread(u32 workerId, const char* name, cThread::ePriority priority)
        {
            std::thread worker([this]()
            {
                sTask task;
                while (true)
                {
                    if (m_TaskBuffer.Pop(task))
                    {
                        // do task and update worker state
                        task.Todo();
                        m_TasksDone.fetch_add(1);
                    }
                    else
                    {
                        // no task, put thread to sleep
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
            task.Type = sTask::eType::PHYSICS;
            task.Todo = [&pxTask]()
            {
                pxTask.run();
                pxTask.release();
            };
            Dispatch(task);
        }

        uint32_t cSimulationDispatcher::getWorkerCount() const
        {
            return GetWorkerCount();
        }

        cTaskDispatcher* MTask::s_AudioDispatcher = nullptr;
        cTaskDispatcher* MTask::s_NetworkDispatcher = nullptr;
        cSimulationDispatcher* MTask::s_SimulationDispatcher = nullptr;
        cTaskDispatcher* MTask::s_ThreadPoolDispatcher = nullptr;

        void MTask::Init()
        {
            s_AudioDispatcher = new cTaskDispatcher(1, 10, "AudioThread", cThread::ePriority::NORMAL);
            s_NetworkDispatcher = new cTaskDispatcher(1, 10, "NetworkThread", cThread::ePriority::NORMAL);
            s_SimulationDispatcher = new cSimulationDispatcher(1, 10, "SimulationThread", cThread::ePriority::NORMAL);
            s_ThreadPoolDispatcher = new cTaskDispatcher(MHardware::CpuInfo.Threads - 3, 100, "ThreadPool", cThread::ePriority::NORMAL);
        }

        void MTask::Free()
        {
            delete s_AudioDispatcher;
            delete s_NetworkDispatcher;
            delete s_SimulationDispatcher;
            delete s_ThreadPoolDispatcher;
        }

        void MTask::Add(const sTask &task)
        {
            switch (task.Type)
            {
                case sTask::eType::AUDIO:
                    s_AudioDispatcher->Dispatch(task);
                    break;
                case sTask::eType::NETWORK:
                    s_NetworkDispatcher->Dispatch(task);
                    break;
                case sTask::eType::PHYSICS:
                    s_SimulationDispatcher->Dispatch(task);
                    break;
                case sTask::eType::ANIMATION:
                    s_SimulationDispatcher->Dispatch(task);
                    break;
                case sTask::eType::THREAD_POOL:
                    s_ThreadPoolDispatcher->Dispatch(task);
                    break;
            }
        }

        cSimulationDispatcher* MTask::GetSimulationDispatcher()
        {
            return s_SimulationDispatcher;
        }
    }
}
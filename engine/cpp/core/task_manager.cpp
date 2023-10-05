namespace xpe {

    namespace core {

        TaskDispatcher* TaskManager::s_Dispatcher = nullptr;

        TaskDispatcher::TaskDispatcher(u32 workerSize, const Thread::Format &threadFormat, usize taskBufferSize)
        {
            m_TasksTodo = 0;
            m_TasksDone.store(0);
            m_TaskBuffer.Resize(taskBufferSize);
            for (int i = 0; i < workerSize ; i++) {
                InitThread(i, threadFormat);
            }
        }

        void TaskDispatcher::Dispatch(const Task &task)
        {
            m_TasksTodo += 1;
            // try to push a new task until it is pushed
            while (!m_TaskBuffer.Push(task)) {
                Poll();
            }
            m_WakeCondition.notify_one();
        }

        void TaskDispatcher::Dispatch(u32 tasksPerThread, u32 taskSize, const Task &task)
        {
            if (taskSize == 0 || tasksPerThread == 0) {
                return;
            }

            u32 taskGroups = (tasksPerThread + taskSize - 1) / taskSize;
            m_TasksTodo += taskGroups;

            for (u32 i = 0; i < taskGroups; ++i) {
                // create task group
                Task taskGroup;
                taskGroup.Runnable = [i, task, taskSize, tasksPerThread]() {
                    u32 groupJobOffset = i * taskSize;
                    u32 groupJobEnd = std::min(groupJobOffset + taskSize, tasksPerThread);
                    for (u32 j = groupJobOffset; j < groupJobEnd; ++j) {
                        task.Runnable();
                    }
                };

                // try to push new group as a task until its pushed
                while (!m_TaskBuffer.Push(taskGroup)) {
                    Poll();
                }

                m_WakeCondition.notify_one();
            }
        }

        bool TaskDispatcher::IsBusy()
        {
            return m_TasksDone.load() < m_TasksTodo;
        }

        void TaskDispatcher::Wait()
        {
            while (IsBusy()) {
                Poll();
            }
        }

        void TaskDispatcher::Poll()
        {
            m_WakeCondition.notify_one();
            std::this_thread::yield();
        }

        void TaskDispatcher::InitThread(u32 workerId, const Thread::Format &threadFormat)
        {
            std::thread worker([this]() {
                Task task;
                while (true) {
                    if (m_TaskBuffer.Pop(task)) {
                        // execute job and update worker label state
                        task.Runnable();
                        m_TasksDone.fetch_add(1);
                    } else {
                        // no job, put thread to sleep
                        std::unique_lock<std::mutex> lock(m_WakeMutex);
                        m_WakeCondition.wait(lock);
                    }
                }
            });
            Thread thread = { workerId, worker };
            thread.SetFormat(threadFormat);
            worker.detach();
        }

        void TaskManager::Init()
        {
            s_Dispatcher = new TaskDispatcher(Hardware::GetCpuStats().Cores, { Thread::ePriority::NORMAL, "Worker" }, 100);
        }

        void TaskManager::Free()
        {
            delete s_Dispatcher;
        }

        void TaskManager::Wait()
        {
            s_Dispatcher->Wait();
        }

        void TaskManager::SubmitTask(const Task &task)
        {
            s_Dispatcher->Dispatch(task);
        }

        void TaskManager::SubmitTask(u32 tasksPerThread, u32 totalTasks, const Task &task)
        {
            s_Dispatcher->Dispatch(tasksPerThread, totalTasks, task);
        }

    }

}
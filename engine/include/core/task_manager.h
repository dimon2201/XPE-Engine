#pragma once

namespace xpe {

    namespace core {

        struct ENGINE_API Task : public Object {
            std::function<void()> Runnable;
        };

        class ENGINE_API TaskDispatcher : public Object {

        public:
            TaskDispatcher(u32 workerSize, usize taskBufferSize, const char* name, Thread::ePriority priority);

            // executes single task in a single worker thread
            void Dispatch(const Task& task);

            // executes multiple tasks with multiple amount of workers per task
            // taskSize - count of inner tasks inside single task
            // tasksPerThread - count of tasks for each worker thread
            void Dispatch(u32 tasksPerThread, u32 taskSize, const Task& task);

            inline bool IsBusy();

            inline u32 GetWorkerCount() const;

            void Wait();

        protected:
            // wakes only one worker
            // allows caller-thread to be rescheduled by OS
            inline void Poll();

            void InitThread(u32 workerId, const char* name, Thread::ePriority priority);

            RingBuffer<Task> m_TaskBuffer;
            std::condition_variable m_WakeCondition;
            std::mutex m_WakeMutex;
            u64 m_TasksTodo;
            std::atomic<u64> m_TasksDone;
            u32 m_WorkerCount;
        };

        class ENGINE_API TaskManager final {

        public:
            static void Init();
            static void Init(TaskDispatcher* dispatcher);
            static void Free();

            static void SubmitTask(const Task& task);
            static void SubmitTask(u32 tasksPerThread, u32 totalTasks, const Task& task);

            static void Wait();

        private:
            static TaskDispatcher* s_Dispatcher;
        };

    }

}
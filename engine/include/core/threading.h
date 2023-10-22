#pragma once

namespace xpe {

    namespace core {

        class ENGINE_API Thread final {

        public:
            enum ePriority {
                LOWEST, NORMAL, HIGHEST
            };

            Thread(u32 id, std::thread& thread);

            static void CurrentSleep(u32 millis);
            static u32 CurrentID();

            void SetFormat(const char* name, ePriority priority);

        private:
            u32 m_ID;
            std::thread& m_Thread;

        };

        struct ENGINE_API Task : public Object {
            std::function<void()> Todo;
            Task* Next = nullptr;

            Task() = default;
            Task(const std::function<void()>& todo) : Todo(todo) {}

            virtual void DoWork();

            void DoAll();
        };

        class ENGINE_API TaskDispatcher : public Object {

        public:
            TaskDispatcher(u32 workerSize, usize taskBufferSize, const char* name, Thread::ePriority priority);

            // executes single task in a single worker thread
            void Dispatch(Task task);

            // executes multiple tasks with multiple amount of workers per task
            // taskSize - count of inner tasks inside single task
            // tasksPerThread - count of tasks for each worker thread
            void Dispatch(u32 tasksPerThread, u32 taskSize, Task task);

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

        class ENGINE_API Threading final {

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
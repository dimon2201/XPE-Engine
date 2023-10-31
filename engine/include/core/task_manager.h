#pragma once

namespace xpe {

    namespace core {

        struct ENGINE_API sTask : public cObject {
            std::function<void()> Todo;
            sTask* Next = nullptr;

            sTask() = default;
            sTask(const std::function<void()>& todo) : Todo(todo) {}

            virtual void DoWork();

            void DoAll();
        };

        class ENGINE_API cTaskDispatcher : public cObject {

        public:
            cTaskDispatcher(u32 workerSize, usize taskBufferSize, const char* name, cThread::ePriority priority);

            // executes single task in a single worker thread
            void Dispatch(sTask task);

            // executes multiple tasks with multiple amount of workers per task
            // taskSize - count of inner tasks inside single task
            // tasksPerThread - count of tasks for each worker thread
            void Dispatch(u32 tasksPerThread, u32 taskSize, sTask task);

            inline bool IsBusy();

            inline u32 GetWorkerCount() const;

            void Wait();

        protected:
            // wakes only one worker
            // allows caller-thread to be rescheduled by OS
            inline void Poll();

            void InitThread(u32 workerId, const char* name, cThread::ePriority priority);

            cRingBuffer<sTask> m_TaskBuffer;
            std::condition_variable m_WakeCondition;
            std::mutex m_WakeMutex;
            u64 m_TasksTodo;
            std::atomic<u64> m_TasksDone;
            u32 m_WorkerCount;
        };

        class ENGINE_API cTaskManager final {

        public:
            static void Init();
            static void Init(cTaskDispatcher* dispatcher);
            static void Free();

            static void SubmitTask(const sTask& task);
            static void SubmitTask(u32 tasksPerThread, u32 totalTasks, const sTask& task);

            static void Wait();

        private:
            static cTaskDispatcher* s_Dispatcher;
        };

    }

}
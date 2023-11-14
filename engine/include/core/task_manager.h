#pragma once

#include <PxPhysicsAPI.h>

namespace xpe {

    namespace core {

        enum eTaskCategory : u8
        {
            AUDIO = 0,
            NETWORK = 1,
            ANIMATION = 2,
            PHYSICS = 3,
            THREAD_POOL = 4
        };

        struct ENGINE_API sTask : public cObject {
            eTaskCategory Category = eTaskCategory::THREAD_POOL;
            std::function<void()> Todo;
            sTask* Next = nullptr;

            sTask() = default;
            sTask(eTaskCategory category, const std::function<void()>& todo) : Category(category), Todo(todo) {}

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

        class ENGINE_API cSimulationDispatcher : public cTaskDispatcher, public physx::PxCpuDispatcher {

        public:
            cSimulationDispatcher(usize workerSize, usize taskBufferSize, const char* name, cThread::ePriority priority)
            : cTaskDispatcher(workerSize, taskBufferSize, name, priority) {}

            ~cSimulationDispatcher() override;

            void submitTask(physx::PxBaseTask& task) override final;

            uint32_t getWorkerCount() const override final;

        };

        class ENGINE_API cTaskManager final {

        public:
            static void Init();
            static void Free();

            static void SubmitTask(const sTask& task);
            static void SubmitTask(u32 tasksPerThread, u32 totalTasks, const sTask& task);

            static cSimulationDispatcher* GetSimulationDispatcher();

        private:
            static cTaskDispatcher* s_AudioDispatcher;
            static cTaskDispatcher* s_NetworkDispatcher;
            static cSimulationDispatcher* s_SimulationDispatcher;
            static cTaskDispatcher* s_ThreadPoolDispatcher;
        };

    }

}
#pragma once

#include <PxPhysicsAPI.h>

namespace xpe {

    namespace core {

        using namespace physx;

        class ENGINE_API MainDispatcher : public TaskDispatcher, public PxCpuDispatcher {

        public:
            MainDispatcher(usize workerSize, usize taskBufferSize, const char* name, Thread::ePriority priority)
            : TaskDispatcher(workerSize, taskBufferSize, name, priority) {}

            ~MainDispatcher() override;

            virtual void submitTask(PxBaseTask& task) override;

            virtual uint32_t getWorkerCount() const override;

        };

    }

}
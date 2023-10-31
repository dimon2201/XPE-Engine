#pragma once

#include <PxPhysicsAPI.h>

namespace xpe {

    namespace core {

        using namespace physx;

        class ENGINE_API cMainDispatcher : public cTaskDispatcher, public PxCpuDispatcher {

        public:
            cMainDispatcher(usize workerSize, usize taskBufferSize, const char* name, cThread::ePriority priority)
            : cTaskDispatcher(workerSize, taskBufferSize, name, priority) {}

            ~cMainDispatcher() override;

            void submitTask(PxBaseTask& task) override final;

            uint32_t getWorkerCount() const override final;

        };

    }

}

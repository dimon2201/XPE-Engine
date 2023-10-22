#pragma once

#include <physics/core/allocator.h>
#include <physics/core/error_callback.h>

namespace xpe {

    namespace physics {

        class ENGINE_API PhysicsManager : public ecs::Manager {

        public:
            static bool EnableMemoryProfiling;

            static void Init(PxCpuDispatcher* dispatcher);
            static void Free();

            static void EnableLoggingInfo(bool enable);
            static void EnableLoggingWarning(bool enable);
            static void EnableLoggingError(bool enable);

        private:
            static PhysicsAllocator s_Allocator;
            static PhysicsErrorCallback s_ErrorCallback;

            static PxFoundation* s_Foundation;
            static PxPhysics* s_Physics;
            static PxCpuDispatcher* s_Dispatcher;

        };

    }

}
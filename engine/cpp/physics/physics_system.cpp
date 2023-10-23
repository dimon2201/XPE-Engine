#include <physics/physics_system.h>

namespace xpe {

    namespace physics {

        bool PhysicsSystem::EnableMemoryProfiling = false;

        PhysicsAllocator PhysicsSystem::s_Allocator;
        PhysicsErrorCallback PhysicsSystem::s_ErrorCallback;

        PxFoundation* PhysicsSystem::s_Foundation = nullptr;
        PxPhysics* PhysicsSystem::s_Physics = nullptr;
        PxCpuDispatcher* PhysicsSystem::s_Dispatcher = nullptr;

        void PhysicsSystem::EnableLoggingInfo(bool enable) {
            s_ErrorCallback.EnableInfoLog = enable;
        }

        void PhysicsSystem::EnableLoggingWarning(bool enable) {
            s_ErrorCallback.EnableWarningLog = enable;
        }

        void PhysicsSystem::EnableLoggingError(bool enable) {
            s_ErrorCallback.EnableErrorLog = enable;
        }

        void PhysicsSystem::Init(PxCpuDispatcher* dispatcher)
        {
            s_Dispatcher = dispatcher;

            s_Foundation = PxCreateFoundation(
                    PX_PHYSICS_VERSION,
                    s_Allocator,
                    s_ErrorCallback
            );

            if (s_Foundation == nullptr) {
                LogError("PhysicsSystem: Failed to initialize PhysXFoundation!");
                return;
            }

            s_Physics = PxCreatePhysics(
                    PX_PHYSICS_VERSION,
                    *s_Foundation,
                    PxTolerancesScale(),
                    EnableMemoryProfiling,
                    nullptr
            );

            if (s_Physics == nullptr) {
                LogError("PhysicsSystem: Failed to initialize PhysXPhysics!");
                return;
            }
        }

        void PhysicsSystem::Free()
        {
            s_Physics->release();
            s_Foundation->release();
        }

    }

}
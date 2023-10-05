#include <physics/physics.h>

namespace xpe {

    namespace physics {

        bool PhysicsManager::EnableMemoryProfiling = false;

        PhysicsAllocator PhysicsManager::s_Allocator;
        PhysicsErrorCallback PhysicsManager::s_ErrorCallback;

        PxFoundation* PhysicsManager::s_Foundation = nullptr;
        PxPhysics* PhysicsManager::s_Physics = nullptr;

        void PhysicsManager::EnableLoggingInfo(bool enable) {
            s_ErrorCallback.EnableInfo = enable;
        }

        void PhysicsManager::EnableLoggingWarning(bool enable) {
            s_ErrorCallback.EnableWarning = enable;
        }

        void PhysicsManager::EnableLoggingError(bool enable) {
            s_ErrorCallback.EnableError = enable;
        }

        void PhysicsManager::Init()
        {
            s_Foundation = PxCreateFoundation(
                    PX_PHYSICS_VERSION,
                    s_Allocator,
                    s_ErrorCallback
            );

            if (s_Foundation == nullptr) {
                LogError("PhysicsManager: Failed to initialize PhysXFoundation!");
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
                LogError("PhysicsManager: Failed to initialize PhysXPhysics!");
                return;
            }
        }

        void PhysicsManager::Free()
        {
            s_Physics->release();
            s_Foundation->release();
        }

    }

}
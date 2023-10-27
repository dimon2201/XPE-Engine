#pragma once

#include <physics/core/allocator.h>
#include <physics/core/error_callback.h>

namespace xpe {

    namespace ecs {
        class Scene;
    }

    namespace physics {

        using namespace xpe::ecs;

        PxFilterFlags FilterShader(
            PxFilterObjectAttributes attributes0, PxFilterData filterData0,
            PxFilterObjectAttributes attributes1, PxFilterData filterData1,
            PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);

        class SimulationEventCallback : public PxSimulationEventCallback {
        public:
            virtual void onConstraintBreak(PxConstraintInfo*, PxU32) override;
            virtual void onWake(PxActor**, PxU32) override;
            virtual void onSleep(PxActor**, PxU32) override;
            virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
            virtual void onTrigger(PxTriggerPair*, PxU32) override;
            virtual void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) override;
        };

        class ENGINE_API PhysicsManager final {

        public:
            static bool EnableMemoryProfiling;

            static void Init(PxCpuDispatcher* dispatcher);
            static void Free();

            static void EnableLoggingInfo(bool enable);
            static void EnableLoggingWarning(bool enable);
            static void EnableLoggingError(bool enable);

        public: 
            static PxRigidDynamic* CreateDynamicActor(glm::vec3 pos);
            static PxMaterial* CreateMaterial(f32 staticFriction, f32 dynamicFriction, f32 restitution);

            static void AddActor(PxRigidDynamic& actor);
            static void Update(Scene* scene, f32 deltaTime);
            static void UpdateCollisions(Scene* scene);

        private:
            static PhysicsAllocator s_Allocator;
            static PhysicsErrorCallback s_ErrorCallback;

            static PxFoundation* s_Foundation;
            static PxPhysics* s_Physics;
            static PxCpuDispatcher* s_Dispatcher;

            static PxScene* s_PhysicsScene;

        private:
            static SimulationEventCallback* eventCallback;
        };

    }

}
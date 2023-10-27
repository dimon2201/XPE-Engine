#include <physics/physics.h>

#include <ecs/scene.h>
#include <ecs/components.hpp>

namespace xpe {

    namespace physics {

        bool PhysicsManager::EnableMemoryProfiling = false;

        PhysicsAllocator PhysicsManager::s_Allocator;
        PhysicsErrorCallback PhysicsManager::s_ErrorCallback;

        PxFoundation* PhysicsManager::s_Foundation = nullptr;
        PxPhysics* PhysicsManager::s_Physics = nullptr;
        PxCpuDispatcher* PhysicsManager::s_Dispatcher = nullptr;
        PxScene* PhysicsManager::s_PhysicsScene = nullptr;
        SimulationEventCallback* PhysicsManager::eventCallback = new SimulationEventCallback;

        void PhysicsManager::EnableLoggingInfo(bool enable) {
            s_ErrorCallback.EnableInfo = enable;
        }

        void PhysicsManager::EnableLoggingWarning(bool enable) {
            s_ErrorCallback.EnableWarning = enable;
        }

        void PhysicsManager::EnableLoggingError(bool enable) {
            s_ErrorCallback.EnableError = enable;
        }

        PxRigidDynamic* PhysicsManager::CreateDynamicActor(glm::vec3 pos)
        {
            PxRigidDynamic* actor = s_Physics->createRigidDynamic(PxTransform(pos.x, pos.y, pos.z));
            return actor;
        }

        PxMaterial* PhysicsManager::CreateMaterial(f32 staticFriction, f32 dynamicFriction, f32 restitution)
        {
            PxMaterial* material = s_Physics->createMaterial(0.5f, 0.5f, 0.5f);
            return material;
        }

        void PhysicsManager::Update(Scene* scene, f32 deltaTime)
        {
            s_PhysicsScene->simulate(deltaTime);
            s_PhysicsScene->fetchResults(true);
            UpdateCollisions(scene);
        }

        void PhysicsManager::UpdateCollisions(Scene* scene)
        {
            scene->EachComponent<DynamicObjectComponent>([](DynamicObjectComponent* component) {
                PxTransform ATransform = component->actor->Actor->getGlobalPose();

                for (auto shape : component->actor->Shapes) {

                    // need push this to another func
                    auto STransform = shape.Shape->getLocalPose();

                    glm::vec3 ShapeEuler = QuatToEuler(ATransform.q.w, ATransform.q.x, ATransform.q.y, ATransform.q.z);

                    shape.Collision->PosisionX.store(ATransform.p.x + STransform.p.x, std::memory_order_relaxed);
                    shape.Collision->PosisionY.store(ATransform.p.y + STransform.p.y, std::memory_order_relaxed);
                    shape.Collision->PosisionZ.store(ATransform.p.z + STransform.p.z, std::memory_order_relaxed);

                    shape.Collision->RotationX.store(ShapeEuler.x, std::memory_order_relaxed);
                    shape.Collision->RotationY.store(ShapeEuler.y, std::memory_order_relaxed);
                    shape.Collision->RotationZ.store(ShapeEuler.z, std::memory_order_relaxed);
                }

            });
        }

        void PhysicsManager::AddActor(PxRigidDynamic& actor)
        {
            s_PhysicsScene->addActor(actor);
        }

        PxFilterFlags FilterShader(
            PxFilterObjectAttributes attributes0, PxFilterData filterData0,
            PxFilterObjectAttributes attributes1, PxFilterData filterData1,
            PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
        {
            // let triggers through
            if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
            {
                pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
                return PxFilterFlag::eDEFAULT;
            }
            // generate contacts for all that were not filtered above
            pairFlags = PxPairFlag::eCONTACT_DEFAULT;

            if ((filterData0.word0 & filterData1.word0) && (filterData1.word1 & filterData0.word1))
                pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

            return PxFilterFlag::eDEFAULT;
        }

        void PhysicsManager::Init(PxCpuDispatcher* dispatcher)
        {
            s_Dispatcher = dispatcher;

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

            PxSceneDesc sceneDesc(s_Physics->getTolerancesScale());
            sceneDesc.gravity = PxVec3(0.0f, 0.0f, 0.0f);//PxVec3(0.0f, -9.81f, 0.0f);
            sceneDesc.cpuDispatcher = dispatcher;
            sceneDesc.filterShader = FilterShader;
            sceneDesc.simulationEventCallback = eventCallback;

            s_PhysicsScene = s_Physics->createScene(sceneDesc);

            if (s_PhysicsScene == nullptr) {
                LogError("PhysicsManager: Failed to initialize PhysXScene!");
                return;
            }
        }

        void PhysicsManager::Free()
        {
            s_PhysicsScene->release();
            s_Physics->release();
            s_Foundation->release();

            delete eventCallback;
        }

        void SimulationEventCallback::onConstraintBreak(PxConstraintInfo*, PxU32)
        {

        }

        void SimulationEventCallback::onWake(PxActor**, PxU32)
        {

        }

        void SimulationEventCallback::onSleep(PxActor**, PxU32)
        {

        }

        void SimulationEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
        {
            LogInfo("onContact");
        }

        void SimulationEventCallback::onTrigger(PxTriggerPair*, PxU32)
        {

        }

        void SimulationEventCallback::onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32)
        {

        }
    }

}
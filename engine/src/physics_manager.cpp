#include <physics_manager.hpp>
#include <physics_allocators.hpp>
#include <physics_callbacks.hpp>
#include <components.hpp>

#include <PxPhysics.h>

#include <gtx/quaternion.hpp>

namespace xpe
{
    namespace physics
    {
        cPhysicsAllocator* MPhysics::s_Allocator = nullptr;
        cPhysicsErrorCallback* MPhysics::s_ErrorCallback = nullptr;
        cPhysicsSimulationEventCallback* MPhysics::s_EventCallback = nullptr;
        PxFoundation* MPhysics::s_Foundation = nullptr;
        PxPhysics* MPhysics::s_Physics = nullptr;
        PxCpuDispatcher* MPhysics::s_Dispatcher = nullptr;
        unordered_map<string, CActor*>* MPhysics::s_Actors = nullptr;
        unordered_map<string, PxScene*>* MPhysics::s_Scenes = nullptr;
        std::mutex MPhysics::s_Mutex;

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
            {
                pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
            }

            return PxFilterFlag::eDEFAULT;
        }

        void MPhysics::Init(PxCpuDispatcher* dispatcher, bool enableMemoryProfiling)
        {
            s_Allocator = new cPhysicsAllocator();
            s_ErrorCallback = new cPhysicsErrorCallback();
            s_EventCallback = new cPhysicsSimulationEventCallback();

            s_Dispatcher = dispatcher;

            s_Foundation = PxCreateFoundation(
                PX_PHYSICS_VERSION,
                *s_Allocator,
                *s_ErrorCallback
            );

            if (s_Foundation == nullptr)
            {
                LogError("MPhysics: Failed to initialize PhysXFoundation!");
                return;
            }

            s_Physics = PxCreatePhysics(
                PX_PHYSICS_VERSION,
                *s_Foundation,
                PxTolerancesScale(),
                enableMemoryProfiling,
                nullptr
            );

            if (s_Physics == nullptr)
            {
                LogError("MPhysics: Failed to initialize PhysXPhysics!");
                return;
            }

            s_Actors = new unordered_map<string, CActor*>;
            s_Scenes = new unordered_map<string, PxScene*>;
        }

        void MPhysics::Free()
        {
            s_Physics->release();
            s_Foundation->release();

            delete s_EventCallback;
        }

        glm::vec3 QuatToEuler(f32 w, f32 x, f32 y, f32 z)
        {
            glm::quat quaternion(w, x, y, z);
            return glm::eulerAngles(quaternion);
        }

        void MPhysics::Update(cScene* scene, float dt)
        {
            if (scene->PhysicsScene == nullptr) return;

            scene->PhysicsScene->simulate(1.0f / 60.0f);
            scene->PhysicsScene->fetchResults(true);

            auto components = scene->GetComponents<CTransform, CActor>();
            for (auto [entity, transform, actor] : components.each())
            {
                PxTransform actorTransform;

                if (transform.UpdatePhysics)
                {
                    SetActorPose(actor, transform);
                }

                switch (actor.ActorType)
                {
                    case eActorType::RIGID_STATIC:
                        actorTransform = ((PxRigidStatic *) actor.Actor)->getGlobalPose();
                        break;

                    case eActorType::RIGID_DYNAMIC:
                        actorTransform = ((PxRigidDynamic *) actor.Actor)->getGlobalPose();
                        break;
                }

                PxShape *shape = actor.Shape;

                glm::vec3 shapeEuler = QuatToEuler(
                        actorTransform.q.w, actorTransform.q.x, actorTransform.q.y, actorTransform.q.z
                );

                // Prevent concurrent access
                {
                    std::lock_guard<std::mutex> lock(s_Mutex);
                    transform.Position = glm::vec3(actorTransform.p.y, actorTransform.p.x, -actorTransform.p.z);
                    transform.Rotation = glm::vec3(shapeEuler.y, shapeEuler.x, -shapeEuler.z);
                }
            }
        }

        void MPhysics::EnableLoggingInfo(bool enable)
        {
            s_ErrorCallback->EnableInfo = enable;
        }

        void MPhysics::EnableLoggingWarning(bool enable)
        {
            s_ErrorCallback->EnableWarning = enable;
        }

        void MPhysics::EnableLoggingError(bool enable)
        {
            s_ErrorCallback->EnableError = enable;
        }

        CActor* MPhysics::AddActor(
            cEntity& entity,
            const eActorType& actorType,
            sShapeDescriptor* shapeDesc,
            const glm::vec3& linearVelocity,
            f32 staticFriction,
            f32 dynamicFriction,
            f32 restitution,
            f32 contactOffset,
            f32 restOffset
        )
        {
            glm::vec3& entityPosition = entity.Get<CTransform>().Position;
            glm::vec3 position = glm::vec3(entityPosition.y, entityPosition.x, -entityPosition.z);
            
            // Creating material
            PxMaterial* physicsMaterial = s_Physics->createMaterial(
                staticFriction, dynamicFriction, restitution
            );

            // Creation of actor
            PxActor* physicsActor = nullptr;
            
            switch (actorType)
            {

                case eActorType::RIGID_STATIC:
                    physicsActor = s_Physics->createRigidStatic(PxTransform(position.x, position.y, position.z));
                    break;

                case eActorType::RIGID_DYNAMIC:
                    physicsActor = s_Physics->createRigidDynamic(PxTransform(position.x, position.y, position.z));
                    ((PxRigidDynamic*)physicsActor)->setAngularDamping(0.75f);
                    ((PxRigidDynamic*)physicsActor)->setLinearVelocity(PxVec3(linearVelocity.x, linearVelocity.y, linearVelocity.z));
                    break;

            }

            if (physicsActor == nullptr)
            {
                return nullptr;
            }

            entity.GetScene()->PhysicsScene->addActor(*physicsActor);

            // Shape creation
            PxShape* physicsShape = nullptr;

            switch (shapeDesc->Type)
            {

                case eShapeType::SPHERE:
                {
                    sSphereShapeDescriptor* sphereShapeDesc = (sSphereShapeDescriptor*)shapeDesc;
                    physicsShape = s_Physics->createShape(
                        PxSphereGeometry(sphereShapeDesc->Radius),
                        *physicsMaterial
                    );
                    break;
                }

                case eShapeType::CAPSULE:
                {
                    sCapsuleShapeDescriptor* capsuleShapeDesc = (sCapsuleShapeDescriptor*)shapeDesc;
                    physicsShape = s_Physics->createShape(
                        PxCapsuleGeometry(capsuleShapeDesc->Radius, capsuleShapeDesc->HalfHeight),
                        *physicsMaterial
                    );
                    break;
                }

                case eShapeType::BOX:
                {
                    sBoxShapeDescriptor* boxShapeDesc = (sBoxShapeDescriptor*)shapeDesc;
                    physicsShape = s_Physics->createShape(
                        PxBoxGeometry(boxShapeDesc->HalfExtents.y, boxShapeDesc->HalfExtents.x, boxShapeDesc->HalfExtents.z),
                        *physicsMaterial
                    );
                    break;
                }

                case eShapeType::PLANE:
                {
                    physicsShape = s_Physics->createShape(
                        PxPlaneGeometry(),
                        *physicsMaterial
                    );
                    break;
                }

            }

            if (physicsShape == nullptr)
            {
                return nullptr;
            }

            //physicsShape->setFlag(PxShapeFlag::eVISUALIZATION, true);
            //physicsShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
            //shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
            //shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);

            physicsShape->setContactOffset(contactOffset);
            physicsShape->setRestOffset(restOffset);

            PxFilterData FilterData;
            FilterData.word0 = 0; FilterData.word1 = 0; FilterData.word2 = 0; FilterData.word3 = 0;
            physicsShape->setSimulationFilterData(FilterData);

            switch (actorType)
            {

                case eActorType::RIGID_STATIC:
                    ((PxRigidStatic*)physicsActor)->attachShape(*physicsShape);
                    break;

                case eActorType::RIGID_DYNAMIC:
                    ((PxRigidDynamic*)physicsActor)->attachShape(*physicsShape);
                    break;

            }

            CActor* actor = &entity.Add<CActor>(
                    physicsActor,
                    physicsMaterial,
                    physicsShape,
                    actorType,
                    shapeDesc,
                    linearVelocity,
                    staticFriction,
                    dynamicFriction,
                    restitution,
                    contactOffset,
                    restOffset
            );

            s_Actors->insert({ entity.GetTag(), actor });

            return actor;
        }

        sRagdoll* MPhysics::AddRagdoll()
        {
            vector<CActor*> bodyparts;

            for (s32 i = 0; i < sRagdoll::k_BodypartCount; i++)
            {
            }

            return nullptr;
        }

        PxScene* MPhysics::AddScene(const string& tag)
        {
            PxSceneDesc sceneDesc(s_Physics->getTolerancesScale());
            sceneDesc.gravity = PxVec3(-1.0f, 0.0f, 0.0f);
            sceneDesc.cpuDispatcher = s_Dispatcher;
            sceneDesc.simulationEventCallback = s_EventCallback;
            sceneDesc.filterShader = FilterShader;

            PxScene* physicsScene = s_Physics->createScene(sceneDesc);

            if (physicsScene == nullptr)
            {
                LogError("MPhysics: Failed to create PhysX scene!");
                return nullptr;
            }

            s_Scenes->insert({ tag, physicsScene });

            return physicsScene;
        }

        void MPhysics::SetActorPose(CActor& actor, CTransform& transform)
        {
            switch (actor.ActorType)
            {

                case eActorType::RIGID_DYNAMIC:
                    glm::quat rotation = glm::quat(transform.Rotation);

                    ((PxRigidDynamic*)actor.Actor)->setGlobalPose(
                        PxTransform(
                            PxVec3(transform.Position.y, transform.Position.x, -transform.Position.z),
                            PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
                        )
                    );
                    break;

            }
        }
    }
}
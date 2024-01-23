#include <PxPhysics.h>

#include <physics/physics_manager.hpp>
#include <physics/allocators.hpp>
#include <physics/callbacks.hpp>

#include <ecs/components.hpp>

#include <rendering/geometry/geometry_manager.hpp>
#include <rendering/material/material_manager.hpp>

#include <gtx/quaternion.hpp>

namespace xpe
{
    namespace physics
    {
        cPhysicsAllocator* cPhysicsManager::s_Allocator = nullptr;
        cPhysicsErrorCallback* cPhysicsManager::s_ErrorCallback = nullptr;
        cPhysicsSimulationEventCallback* cPhysicsManager::s_EventCallback = nullptr;
        PxFoundation* cPhysicsManager::s_Foundation = nullptr;
        PxPhysics* cPhysicsManager::s_Physics = nullptr;
        PxCpuDispatcher* cPhysicsManager::s_Dispatcher = nullptr;
        unordered_map<string, sActor*>* cPhysicsManager::s_Actors = nullptr;
        unordered_map<string, PxScene*>* cPhysicsManager::s_Scenes = nullptr;

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

        void cPhysicsManager::Init(PxCpuDispatcher* dispatcher, bool enableMemoryProfiling)
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

            if (s_Foundation == nullptr) {
                LogError("cPhysicsManager: Failed to initialize PhysXFoundation!");
                return;
            }

            s_Physics = PxCreatePhysics(
                PX_PHYSICS_VERSION,
                *s_Foundation,
                PxTolerancesScale(),
                enableMemoryProfiling,
                nullptr
            );

            if (s_Physics == nullptr) {
                LogError("cPhysicsManager: Failed to initialize PhysXPhysics!");
                return;
            }

            s_Actors = new unordered_map<string, sActor*>;
            s_Scenes = new unordered_map<string, PxScene*>;
        }

        void cPhysicsManager::Free()
        {
            s_Physics->release();
            s_Foundation->release();

            delete s_EventCallback;
        }

        void cPhysicsManager::EnableLoggingInfo(bool enable) {
            s_ErrorCallback->EnableInfo = enable;
        }

        void cPhysicsManager::EnableLoggingWarning(bool enable) {
            s_ErrorCallback->EnableWarning = enable;
        }

        void cPhysicsManager::EnableLoggingError(bool enable) {
            s_ErrorCallback->EnableError = enable;
        }

        sActor* cPhysicsManager::AddActor(
            cEntity& entity,
            const sActor::eActorType& actorType,
            sShapeDescriptor* shapeDesc,
            const glm::vec3& linearVelocity,
            f32 staticFriction,
            f32 dynamicFriction,
            f32 restitution,
            f32 contactOffset,
            f32 restOffset,
            f32 mass
        )
        {
            glm::vec3& entityPosition = entity.GetPosition();
            glm::vec3 position = glm::vec3(entityPosition.y, entityPosition.x, -entityPosition.z);
            
            // Creating material
            PxMaterial* physicsMaterial = s_Physics->createMaterial(
                staticFriction, dynamicFriction, restitution
            );

            // Creation of actor
            PxActor* physicsActor = nullptr;
            
            switch (actorType)
            {

            case sActor::eActorType::RIGID_STATIC:
                physicsActor = s_Physics->createRigidStatic(PxTransform(position.x, position.y, position.z));
                break;

            case sActor::eActorType::RIGID_DYNAMIC:
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

            case sActor::eActorType::RIGID_STATIC:
                ((PxRigidStatic*)physicsActor)->attachShape(*physicsShape);
                break;

            case sActor::eActorType::RIGID_DYNAMIC:
                ((PxRigidDynamic*)physicsActor)->attachShape(*physicsShape);
                break;

            }

            // Return
            sActor* actor = new sActor(
                &entity,
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
                restOffset,
                mass
            );

            s_Actors->insert({ entity.GetTag(), actor });

            return actor;
        }

        sJoint* cPhysicsManager::AddJoint(
            sActor* actor1,
            const glm::vec3& localPosition1,
            sActor* actor2,
            const glm::vec3& localPosition2
        )
        {
            PxSphericalJoint* physicalJoint = PxSphericalJointCreate(
                *s_Physics,
                (PxRigidActor*)actor1->Actor,
                PxTransform(PxVec3(localPosition1.y, localPosition1.x, -localPosition1.z)),
                (PxRigidActor*)actor2->Actor,
                PxTransform(PxVec3(localPosition2.y, localPosition2.x, -localPosition2.z))
            );

            physicalJoint->setConstraintFlag(PxConstraintFlag::eCOLLISION_ENABLED, true);

            ((PxRigidBody*)actor1->Actor)->setLinearDamping(1.0f);
            ((PxRigidBody*)actor2->Actor)->setLinearDamping(1.0f);
            //((PxRigidBody*)actor1->Actor)->setAngularDamping(0.25f);
            //((PxRigidBody*)actor2->Actor)->setAngularDamping(0.25f);

            sJoint* joint = new sJoint(physicalJoint, actor1, actor2);

            return joint;
        }

        sRagdoll* cPhysicsManager::AddRagdoll(cScene* scene, sSkeleton* skeleton)
        {
            vector<sActor*> bodyparts;
            vector<sJoint*> joints;

            for (auto& it = skeleton->Bones.begin(); it != skeleton->Bones.end(); it++)
            {
                sBone* bone = &it->second;
                string name = bone->Name;
                glm::vec3 offset = glm::vec3(bone->Offset[3][0], bone->Offset[3][1], bone->Offset[3][2]);
                offset += 5.0f;

                // Set bodypart type based on bone name
                sRagdoll::eBodypart bodypartType = sRagdoll::NONE;
                if (name == "Armature_Root") { bodypartType = sRagdoll::ROOT; }
                if (name == "Armature_Spine1") { bodypartType = sRagdoll::SPINE1; }
                if (name == "Armature_Spine2") { bodypartType = sRagdoll::SPINE2; }
                if (name == "Armature_Neck") { bodypartType = sRagdoll::NECK; }
                if (name == "Armature_Head") { bodypartType = sRagdoll::HEAD; }
                if (name == "Armature_ShoulderL") { bodypartType = sRagdoll::SHOULDER_LEFT; }
                if (name == "Armature_UpperarmL") { bodypartType = sRagdoll::UPPERARM_LEFT; }
                if (name == "Armature_LowerarmL") { bodypartType = sRagdoll::LOWERARM_LEFT; }
                if (name == "Armature_ArmL") { bodypartType = sRagdoll::ARM_LEFT; }
                if (name == "Armature_ShoulderR") { bodypartType = sRagdoll::SHOULDER_RIGHT; }
                if (name == "Armature_UpperarmR") { bodypartType = sRagdoll::UPPERARM_RIGHT; }
                if (name == "Armature_LowerarmR") { bodypartType = sRagdoll::LOWERARM_RIGHT; }
                if (name == "Armature_ArmR") { bodypartType = sRagdoll::ARM_RIGHT; }
                if (name == "Armature_UpperlegL") { bodypartType = sRagdoll::UPPERLEG_LEFT; }
                if (name == "Armature_LowerlegL") { bodypartType = sRagdoll::LOWERLEG_LEFT; }
                if (name == "Armature_FootL") { bodypartType = sRagdoll::FOOT_LEFT; }
                if (name == "Armature_UpperlegR") { bodypartType = sRagdoll::UPPERLEG_RIGHT; }
                if (name == "Armature_LowerlegR") { bodypartType = sRagdoll::LOWERLEG_RIGHT; }
                if (name == "Armature_FootR") { bodypartType = sRagdoll::FOOT_RIGHT; }

                if (bodypartType == sRagdoll::NONE) {
                    continue;
                }

                // Create bodypart entity
                cEntity* entity = new cEntity("RagdollBodypart" + bone->Name, scene);
                entity->SetPosition(glm::vec3(1.0f));
                entity->SetScale(glm::vec3(0.1f));
                entity->Add<CGeometryInfo>(cGeometryManager::AddGeometry(sSphere()).second);

                sCapsuleShapeDescriptor capsuleShapeDesc(0.5f, 0.5f);
                sActor* actor = cPhysicsManager::AddActor(
                    *entity,
                    CPhysicsActor::eActorType::RIGID_DYNAMIC,
                    &capsuleShapeDesc,
                    glm::vec3(0.0f),
                    0.5f, 0.5f, 0.5f,
                    0.05f, 0.0f,
                    10.0f
                );

                entity->Add<CPhysicsActor>(
                    actor
                );

                bodyparts.push_back(actor);

                sTransform transform;
                transform.Position = offset;
                transform.Rotation = glm::vec3(0.0f);
                transform.Scale = glm::vec3(1.0f);

                //bodypart->ParentEntity->SetPosition(transform.Position);

                LogWarning(
                    bone->Name + "; " +
                    string(std::to_string(transform.Position.x)) + "; " +
                    string(std::to_string(transform.Position.y)) + "; " +
                    string(std::to_string(transform.Position.z))
                );

                //cPhysicsManager::SetActorPose(*bodypart, transform);
            }

            for (s32 i = 0; i < sRagdoll::k_BodypartCount; i++)
            {
                cEntity* entity = new cEntity("RagdollBodypart" + i, scene);

                entity->SetPosition(glm::vec3(1.0f));
                entity->SetScale(glm::vec3(0.1f));
                entity->Add<CMaterial2>();
                entity->Add<CGeometryInfo>(cGeometryManager::AddGeometry(sSphere()).second);

                sCapsuleShapeDescriptor capsuleShapeDesc(0.5f, 0.5f);
                sActor* actor = cPhysicsManager::AddActor(
                    *entity,
                    CPhysicsActor::eActorType::RIGID_DYNAMIC,
                    &capsuleShapeDesc,
                    glm::vec3(0.0f),
                    0.5f, 0.5f, 0.5f,
                    0.05f, 0.0f,
                    10.0f
                );
                
                entity->Add<CPhysicsActor>(
                   actor
                );

                bodyparts.push_back(actor);
            }

            // Root-Spine1 joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::ROOT],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::SPINE1],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Spine1-Spine2 joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::SPINE1],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::SPINE2],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Spine2-Neck joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::SPINE2],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::NECK],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Neck-Head joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::NECK],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::HEAD],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Spine2-Left shoulder joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::SPINE2],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::SHOULDER_LEFT],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Left shoulder-Left upperarm joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::SHOULDER_LEFT],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::UPPERARM_LEFT],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Left upperarm-Left lowerarm joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::UPPERARM_LEFT],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::LOWERARM_LEFT],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Left lowerarm-Left arm joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::LOWERARM_LEFT],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::ARM_LEFT],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Spine2-Right shoulder joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::SPINE2],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::SHOULDER_RIGHT],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Right shoulder-Right upperarm joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::SHOULDER_RIGHT],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::UPPERARM_RIGHT],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Right upperarm-Right lowerarm joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::UPPERARM_RIGHT],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::LOWERARM_RIGHT],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Right lowerarm-Right arm joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::LOWERARM_RIGHT],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::ARM_RIGHT],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Root-Left upperleg joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::ROOT],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::UPPERLEG_LEFT],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Left upperleg-Left lowerleg joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::UPPERLEG_LEFT],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::LOWERLEG_LEFT],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Left lowerleg-Left foot joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::LOWERLEG_LEFT],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::FOOT_LEFT],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Root-Right upperleg joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::ROOT],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::UPPERLEG_RIGHT],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Right upperleg-Right lowerleg joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::UPPERLEG_RIGHT],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::LOWERLEG_RIGHT],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            // Right lowerleg-Right foot joint
            joints.push_back(cPhysicsManager::AddJoint(
                bodyparts[sRagdoll::LOWERLEG_RIGHT],
                glm::vec3(0.0f, 0.0f, 0.0f),
                bodyparts[sRagdoll::FOOT_RIGHT],
                glm::vec3(0.0f, 0.0f, 0.0f)
            ));

            return new sRagdoll(bodyparts, joints);
        }

        PxScene* cPhysicsManager::AddScene(const string& tag)
        {
            PxSceneDesc sceneDesc(s_Physics->getTolerancesScale());
            sceneDesc.gravity = PxVec3(-9.81f, 0.0f, 0.0f);
            sceneDesc.cpuDispatcher = s_Dispatcher;
            sceneDesc.simulationEventCallback = s_EventCallback;
            sceneDesc.filterShader = FilterShader;

            PxScene* physicsScene = s_Physics->createScene(sceneDesc);

            if (physicsScene == nullptr)
            {
                LogError("cPhysicsManager: Failed to create PhysX scene!");
                return nullptr;
            }

            s_Scenes->insert({ tag, physicsScene });

            return physicsScene;
        }

        void cPhysicsManager::ConstructRagdollJoints(sSkeleton* skeleton, sRagdoll* ragdoll)
        {
            for (auto& it = skeleton->Bones.begin(); it != skeleton->Bones.end(); it++)
            {
                sBone* bone = &it->second;

                glm::vec3 offset = glm::vec3(bone->Offset[3][0], bone->Offset[3][1], bone->Offset[3][2]);
                offset += 5.0f;

                sActor* bodypart = cPhysicsManager::GetRagdollBodypartByName(ragdoll, bone->Name);
                if (bodypart == nullptr) {
                    continue;
                }

                sTransform transform;
                transform.Position = offset;
                transform.Rotation = glm::vec3(0.0f);
                transform.Scale = glm::vec3(1.0f);

                bodypart->ParentEntity->SetPosition(transform.Position);

                LogWarning(
                    bone->Name + "; " +
                    string(std::to_string(transform.Position.x)) + "; " +
                    string(std::to_string(transform.Position.y)) + "; " +
                    string(std::to_string(transform.Position.z))
                );

                cPhysicsManager::SetActorPose(*bodypart, transform);
            }

            sActor* bpRoot = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_Root");
            sActor* bpSpine1 = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_Spine1");
            sActor* bpSpine2 = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_Spine2");
            sActor* bpNeck = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_Neck");
            sActor* bpHead = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_Head");
            sActor* bpShoulderLeft = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_ShoulderL");
            sActor* bpUpperarmLeft = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_UpperarmL");
            sActor* bpLowerarmLeft = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_LowerarmL");
            sActor* bpArmLeft = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_ArmL");
            sActor* bpShoulderRight = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_ShoulderR");
            sActor* bpUpperarmRight = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_UpperarmR");
            sActor* bpLowerarmRight = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_LowerarmR");
            sActor* bpArmRight = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_ArmR");
            sActor* bpUpperlegLeft = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_UpperlegL");
            sActor* bpLowerlegLeft = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_LowerlegL");
            sActor* bpFootLeft = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_FootL");
            sActor* bpUpperlegRight = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_UpperlegR");
            sActor* bpLowerlegRight = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_LowerlegR");
            sActor* bpFootRight = cPhysicsManager::GetRagdollBodypartByName(ragdoll, "Armature_FootR");

            // Root-Spine1 joint
            SetJointPose(ragdoll->Joints[0], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[0], 1, bpRoot->ParentEntity->GetPosition() - bpSpine1->ParentEntity->GetPosition());

            // Spine1-Spine2 joint
            SetJointPose(ragdoll->Joints[1], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[1], 1, bpSpine1->ParentEntity->GetPosition() - bpSpine2->ParentEntity->GetPosition());

            // Spine2-Neck joint
            SetJointPose(ragdoll->Joints[2], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[2], 1, bpSpine2->ParentEntity->GetPosition() - bpNeck->ParentEntity->GetPosition());

            // Neck-Head joint
            SetJointPose(ragdoll->Joints[3], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[3], 1, bpNeck->ParentEntity->GetPosition() - bpHead->ParentEntity->GetPosition());

            // Spine2-Left shoulder joint
            SetJointPose(ragdoll->Joints[4], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[4], 1, bpSpine2->ParentEntity->GetPosition() - bpShoulderLeft->ParentEntity->GetPosition());

            // Left shoulder-Left upperarm joint
            SetJointPose(ragdoll->Joints[5], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[5], 1, bpShoulderLeft->ParentEntity->GetPosition() - bpUpperarmLeft->ParentEntity->GetPosition());

            // Left upperarm-Left lowerarm joint
            SetJointPose(ragdoll->Joints[6], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[6], 1, bpUpperarmLeft->ParentEntity->GetPosition() - bpLowerarmLeft->ParentEntity->GetPosition());

            // Left lowerarm-Left arm joint
            SetJointPose(ragdoll->Joints[7], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[7], 1, bpLowerarmLeft->ParentEntity->GetPosition() - bpArmLeft->ParentEntity->GetPosition());

            // Spine2-Right shoulder joint
            SetJointPose(ragdoll->Joints[8], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[8], 1, bpSpine2->ParentEntity->GetPosition() - bpShoulderRight->ParentEntity->GetPosition());

            // Right shoulder-Right upperarm joint
            SetJointPose(ragdoll->Joints[9], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[9], 1, bpShoulderRight->ParentEntity->GetPosition() - bpUpperarmRight->ParentEntity->GetPosition());

            // Right upperarm-Right lowerarm joint
            SetJointPose(ragdoll->Joints[10], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[10], 1, bpUpperarmRight->ParentEntity->GetPosition() - bpLowerarmRight->ParentEntity->GetPosition());

            // Right lowerarm-Right arm joint
            SetJointPose(ragdoll->Joints[11], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[11], 1, bpLowerarmRight->ParentEntity->GetPosition() - bpArmRight->ParentEntity->GetPosition());

            // Root-Left upperleg joint
            SetJointPose(ragdoll->Joints[12], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[12], 1, bpRoot->ParentEntity->GetPosition() - bpUpperlegLeft->ParentEntity->GetPosition());

            // Left upperleg-Left lowerleg joint
            SetJointPose(ragdoll->Joints[13], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[13], 1, bpUpperlegLeft->ParentEntity->GetPosition() - bpLowerlegLeft->ParentEntity->GetPosition());

            // Left lowerleg-Left foot joint
            SetJointPose(ragdoll->Joints[14], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[14], 1, bpLowerlegLeft->ParentEntity->GetPosition() - bpFootLeft->ParentEntity->GetPosition());

            // Root-Right upperleg joint
            SetJointPose(ragdoll->Joints[15], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[15], 1, bpRoot->ParentEntity->GetPosition() - bpUpperlegRight->ParentEntity->GetPosition());

            // Right upperleg-Right lowerleg joint
            SetJointPose(ragdoll->Joints[16], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[16], 1, bpUpperlegRight->ParentEntity->GetPosition() - bpLowerlegRight->ParentEntity->GetPosition());

            // Right lowerleg-Right foot joint
            SetJointPose(ragdoll->Joints[17], 0, glm::vec3(0.0f));
            SetJointPose(ragdoll->Joints[17], 1, bpLowerlegRight->ParentEntity->GetPosition() - bpFootRight->ParentEntity->GetPosition());
        }

        void cPhysicsManager::SetActorPose(sActor& actor, sTransform& transform)
        {
            switch (actor.ActorType)
            {

            case sActor::eActorType::RIGID_DYNAMIC:
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

        void cPhysicsManager::SetJointPose(sJoint* joint, u32 actorIndex, const glm::vec3& position)
        {
            if (actorIndex == 0)
            {
                joint->Joint->setLocalPose(
                    PxJointActorIndex::eACTOR0,
                    PxTransform(PxVec3(position.y, position.x, -position.z))
                );
            }
            else if (actorIndex == 1)
            {
                joint->Joint->setLocalPose(
                    PxJointActorIndex::eACTOR1,
                    PxTransform(PxVec3(position.y, position.x, -position.z))
                );
            }
        }

        void cPhysicsManager::SetForce(sActor* actor, const glm::vec3& force)
        {
            switch (actor->ActorType)
            {

            case sActor::eActorType::RIGID_DYNAMIC:
                ((PxRigidDynamic*)actor->Actor)->setForceAndTorque(
                    PxVec3(force.y, force.x, -force.z),
                    PxVec3(0.0f, 0.0f, 0.0f)
                );
                break;

            }
        }

        sActor* cPhysicsManager::GetRagdollBodypartByName(sRagdoll* ragdoll, const string& name)
        {
            sActor* bodypart = nullptr;

            if (name == "Armature_Root") { return ragdoll->Bodyparts[sRagdoll::ROOT]; }
            if (name == "Armature_Spine1") { return ragdoll->Bodyparts[sRagdoll::SPINE1]; }
            if (name == "Armature_Spine2") { return ragdoll->Bodyparts[sRagdoll::SPINE2]; }
            if (name == "Armature_Neck") { return ragdoll->Bodyparts[sRagdoll::NECK]; }
            if (name == "Armature_Head") { return ragdoll->Bodyparts[sRagdoll::HEAD]; }
            if (name == "Armature_ShoulderL") { return ragdoll->Bodyparts[sRagdoll::SHOULDER_LEFT]; }
            if (name == "Armature_UpperarmL") { return ragdoll->Bodyparts[sRagdoll::UPPERARM_LEFT]; }
            if (name == "Armature_LowerarmL") { return ragdoll->Bodyparts[sRagdoll::LOWERARM_LEFT]; }
            if (name == "Armature_ArmL") { return ragdoll->Bodyparts[sRagdoll::ARM_LEFT]; }
            if (name == "Armature_ShoulderR") { return ragdoll->Bodyparts[sRagdoll::SHOULDER_RIGHT]; }
            if (name == "Armature_UpperarmR") { return ragdoll->Bodyparts[sRagdoll::UPPERARM_RIGHT]; }
            if (name == "Armature_LowerarmR") { return ragdoll->Bodyparts[sRagdoll::LOWERARM_RIGHT]; }
            if (name == "Armature_ArmR") { return ragdoll->Bodyparts[sRagdoll::ARM_RIGHT]; }
            if (name == "Armature_UpperlegL") { return ragdoll->Bodyparts[sRagdoll::UPPERLEG_LEFT]; }
            if (name == "Armature_LowerlegL") { return ragdoll->Bodyparts[sRagdoll::LOWERLEG_LEFT]; }
            if (name == "Armature_FootL") { return ragdoll->Bodyparts[sRagdoll::FOOT_LEFT]; }
            if (name == "Armature_UpperlegR") { return ragdoll->Bodyparts[sRagdoll::UPPERLEG_RIGHT]; }
            if (name == "Armature_LowerlegR") { return ragdoll->Bodyparts[sRagdoll::LOWERLEG_RIGHT]; }
            if (name == "Armature_FootR") { return ragdoll->Bodyparts[sRagdoll::FOOT_RIGHT]; }

            return nullptr;
        }

        glm::mat4 cPhysicsManager::GetRagdollBodypartMatrix(sRagdoll* ragdoll, const string& name)
        {
            sActor* bodypart = GetRagdollBodypartByName(ragdoll, name);

            if (bodypart == nullptr) {
                return glm::mat4(1.0f);
            }

            return glm::toMat4(glm::quat(bodypart->ParentEntity->GetRotation()));
        }

    }

}
#pragma once

namespace physx
{
    class PxActor;
    class PxRigidDynamic;
    class PxShape;
    class PxMaterial;
    class PxJoint;
    class PxScene;
    class PxFoundation;
    class PxPhysics;
    class PxCpuDispatcher;
}

namespace xpe {

    namespace core
    {
        class cMainDispatcher;
    }

    namespace anim
    {
        struct sSkeleton;
    }

    namespace ecs
    {
        class cEntity;
        class cScene;
    }

    namespace render
    {
        class sMaterial;
    }

    namespace math
    {
        struct sTransform;
    }

    namespace physics
    {
        using namespace physx;
        using namespace core;
        using namespace anim;
        using namespace ecs;

        class cPhysicsAllocator;
        class cPhysicsErrorCallback;
        class cPhysicsSimulationEventCallback;

        enum class eShapeType
        {
            NONE = 0,
            SPHERE = 1,
            CAPSULE = 2,
            BOX = 3,
            PLANE = 4
        };

        JSON_ENUM(eShapeType, {
            { eShapeType::NONE, "NONE" },
            { eShapeType::SPHERE, "SPHERE" },
            { eShapeType::CAPSULE, "CAPSULE" },
            { eShapeType::BOX, "BOX" },
            { eShapeType::PLANE, "PLANE" }
        })

        struct ENGINE_API sShapeDescriptor : public cObject
        {
            sShapeDescriptor(const eShapeType& type)
            {
                Type = type;
            }

            eShapeType Type;
        };
        JSON(sShapeDescriptor, Type)

        struct ENGINE_API sSphereShapeDescriptor : public sShapeDescriptor
        {
            sSphereShapeDescriptor() = delete;
            sSphereShapeDescriptor(f32 radius) : sShapeDescriptor(eShapeType::SPHERE), Radius(radius) {}

            f32 Radius;
        };
        JSON(sSphereShapeDescriptor, Type, Radius)

        struct ENGINE_API sCapsuleShapeDescriptor : public sShapeDescriptor
        {
            sCapsuleShapeDescriptor() = delete;
            sCapsuleShapeDescriptor(f32 radius, f32 halfHeight)
            : sShapeDescriptor(eShapeType::CAPSULE), Radius(radius), HalfHeight(halfHeight) {}

            f32 Radius;
            f32 HalfHeight;
        };
        JSON(sCapsuleShapeDescriptor, Type, Radius, HalfHeight)

        struct ENGINE_API sBoxShapeDescriptor : public sShapeDescriptor
        {
            sBoxShapeDescriptor() = delete;
            sBoxShapeDescriptor(const glm::vec3& halfExtents)
            : sShapeDescriptor(eShapeType::BOX), HalfExtents(halfExtents) {}

            glm::vec3 HalfExtents;
        };
        JSON(sBoxShapeDescriptor, Type, HalfExtents)

        struct ENGINE_API sPlaneShapeDescriptor : public sShapeDescriptor
        {
            sPlaneShapeDescriptor() : sShapeDescriptor(eShapeType::PLANE) {}
        };
        JSON(sPlaneShapeDescriptor, Type)

        struct ENGINE_API sActor
        {
            enum class eActorType
            {
                NONE = 0,
                RIGID_STATIC = 1,
                RIGID_DYNAMIC = 2
            };
            JSON_ENUM(eActorType, {
                { eActorType::NONE,          "NONE" },
                { eActorType::RIGID_STATIC,  "RIGID_STATIC" },
                { eActorType::RIGID_DYNAMIC, "RIGID_DYNAMIC" },
            })

            sActor() = default;

            sActor(
                cEntity* entity,
                PxActor* actor,
                PxMaterial* material,
                PxShape* shape,
                const eActorType& actorType,
                sShapeDescriptor* shapeDesc,
                const glm::vec3& linearVelocity,
                f32 staticFriction,
                f32 dynamicFriction,
                f32 restitution,
                f32 contactOffset,
                f32 restOffset,
                f32 mass
            ) :
                ParentEntity(entity),
                Actor(actor),
                Material(material),
                Shape(shape),
                ActorType(actorType),
                ShapeDesc(*shapeDesc),
                LinearVelocity(linearVelocity),
                StaticFriction(staticFriction),
                DynamicFriction(dynamicFriction),
                Restitution(restitution),
                ContactOffset(contactOffset),
                RestOffset(restOffset),
                Mass(mass)
            {
            }

            cEntity* ParentEntity = nullptr;
            PxActor* Actor = nullptr;
            PxMaterial* Material = nullptr;
            PxShape* Shape = nullptr;
            eActorType ActorType = eActorType::NONE;
            sShapeDescriptor ShapeDesc = sShapeDescriptor(eShapeType::NONE);
            glm::vec3 LinearVelocity = glm::vec3(0.0f);
            f32 StaticFriction = 0.0f;
            f32 DynamicFriction = 0.0f;
            f32 Restitution = 0.0f;
            f32 ContactOffset = 0.0f;
            f32 RestOffset = 0.0f;
            f32 Mass = 0.0f;
        };

        struct ENGINE_API sJoint : public cObject
        {
            sJoint(PxSphericalJoint* joint, sActor* actor1, sActor* actor2)
            {
                Joint = joint;
                Actor1 = actor1;
                Actor2 = actor2;
            }

            PxSphericalJoint* Joint = nullptr;
            sActor* Actor1 = nullptr;
            sActor* Actor2 = nullptr;
        };

        struct ENGINE_API sRagdoll : public cObject
        {
            sRagdoll(const vector<sActor*>& bodyparts, const vector<sJoint*>& joints)
                : Bodyparts(bodyparts), Joints(joints)
            {}
            ~sRagdoll() = default;

            static const usize k_BodypartCount = 20;

            enum eBodypart
            {
                ROOT = 0,
                SPINE1 = 1,
                SPINE2 = 2,
                NECK = 3,
                HEAD = 4,
                SHOULDER_LEFT = 5,
                UPPERARM_LEFT = 6,
                LOWERARM_LEFT = 7,
                ARM_LEFT = 8,
                SHOULDER_RIGHT = 9,
                UPPERARM_RIGHT = 10,
                LOWERARM_RIGHT = 11,
                ARM_RIGHT = 12,
                UPPERLEG_LEFT = 13,
                LOWERLEG_LEFT = 14,
                FOOT_LEFT = 15,
                UPPERLEG_RIGHT = 16,
                LOWERLEG_RIGHT = 17,
                FOOT_RIGHT = 18,
                NONE = 19
            };

            vector<sActor*> Bodyparts;
            vector<sJoint*> Joints;
        };

        class ENGINE_API cPhysicsManager final
        {

        public:
            static void Init(PxCpuDispatcher* dispatcher, bool enableMemoryProfiling = false);
            static void Free();

            static void EnableLoggingInfo(bool enable);
            static void EnableLoggingWarning(bool enable);
            static void EnableLoggingError(bool enable);

            static sActor* AddActor(
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
            );
            static sJoint* AddJoint(
                sActor* actor1,
                const glm::vec3& localPosition1,
                sActor* actor2,
                const glm::vec3& localPosition2
            );
            static sRagdoll* AddRagdoll(cScene* scene, sSkeleton* skeleton);
            static PxScene* AddScene(const string& tag);

            static void ConstructRagdollJoints(sSkeleton* skeleton, sRagdoll* ragdoll);

            static void SetActorPose(sActor& actor, math::sTransform& transform);
            static void SetJointPose(sJoint* joint, u32 actorIndex, const glm::vec3& position);
            static void SetForce(sActor* actor, const glm::vec3& force);
            static sActor* GetRagdollBodypartByName(sRagdoll* ragdoll, const string& name);
            static glm::mat4 GetRagdollBodypartMatrix(sRagdoll* ragdoll, const string& name);

        private:
            static cPhysicsAllocator* s_Allocator;
            static cPhysicsErrorCallback* s_ErrorCallback;
            static cPhysicsSimulationEventCallback* s_EventCallback;

            static PxFoundation* s_Foundation;
            static PxPhysics* s_Physics;
            static PxCpuDispatcher* s_Dispatcher;

            static unordered_map<string, sActor*>* s_Actors;
            static unordered_map<string, PxScene*>* s_Scenes;
        };

    }

}
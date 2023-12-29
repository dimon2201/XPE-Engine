#pragma once

namespace physx
{
    class PxActor;
    class PxRigidDynamic;
    class PxShape;
    class PxMaterial;
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
                f32 restOffset
            ) :
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
                RestOffset(restOffset)
            {
            }

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
        };

        struct ENGINE_API sRagdoll : public cObject
        {
            sRagdoll(const vector<sActor*>& bodyparts) : Bodyparts(bodyparts) {}
            ~sRagdoll() = default;

            static const usize k_BodypartCount = 12;

            enum eBodypart
            {
                NONE = 0,
                HEAD = 1,
                TORSO_TOP = 2,
                TORSO_MIDDLE = 3,
                TORSO_BOTTOM = 4,
                LEFT_ARM_TOP = 5,
                LEFT_ARM_BOTTOM = 6,
                RIGHT_ARM_TOP = 7,
                RIGHT_ARM_BOTTOM = 8,
                LEFT_LEG_TOP = 9,
                LEFT_LEG_BOTTOM = 10,
                RIGHT_LEG_TOP = 11,
                RIGHT_LEG_BOTTOM = 12
            };

            vector<sActor*> Bodyparts;
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
                f32 restOffset
            );
            
            static sRagdoll* AddRagdoll();
            static PxScene* AddScene(const string& tag);

            static void SetActorPose(sActor& actor, math::sTransform& transform);

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
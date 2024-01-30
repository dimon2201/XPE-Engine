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

namespace xpe::ecs
{
    class cScene;
    class cEntity;
    class CTransform;
    class CActor;
}

namespace xpe
{
    namespace physics
    {
        using namespace physx;

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

        JSON_ENUM(eShapeType,
        {
            { eShapeType::NONE, "NONE" },
            { eShapeType::SPHERE, "SPHERE" },
            { eShapeType::CAPSULE, "CAPSULE" },
            { eShapeType::BOX, "BOX" },
            { eShapeType::PLANE, "PLANE" }
        })

        struct ENGINE_API sShapeDescriptor : public cObject
        {
            eShapeType Type;

            sShapeDescriptor(const eShapeType& type)
            {
                Type = type;
            }
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

        enum class eActorType
        {
            NONE = 0,
            RIGID_STATIC = 1,
            RIGID_DYNAMIC = 2
        };
        JSON_ENUM(eActorType,
        {
            { eActorType::NONE,          "NONE" },
            { eActorType::RIGID_STATIC,  "RIGID_STATIC" },
            { eActorType::RIGID_DYNAMIC, "RIGID_DYNAMIC" },
        })

        struct ENGINE_API sRagdoll : public cObject
        {
            static const usize k_BodypartCount = 12;
            vector<CActor*> Bodyparts;

            sRagdoll(const vector<CActor*>& bodyparts) : Bodyparts(bodyparts) {}
            ~sRagdoll() = default;

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
        };

        class ENGINE_API MPhysics final
        {

        public:
            static void Init(PxCpuDispatcher* dispatcher, bool enableMemoryProfiling = false);
            static void Free();
            static void Update(cScene* scene, float dt);

            static void EnableLoggingInfo(bool enable);
            static void EnableLoggingWarning(bool enable);
            static void EnableLoggingError(bool enable);

            static CActor* AddActor(
                cEntity& entity,
                const eActorType& actorType,
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

            static void SetActorPose(CActor& actor, CTransform& transform);

        private:
            static cPhysicsAllocator* s_Allocator;
            static cPhysicsErrorCallback* s_ErrorCallback;
            static cPhysicsSimulationEventCallback* s_EventCallback;

            static PxFoundation* s_Foundation;
            static PxPhysics* s_Physics;
            static PxCpuDispatcher* s_Dispatcher;

            static unordered_map<string, CActor*>* s_Actors;
            static unordered_map<string, PxScene*>* s_Scenes;
            static std::mutex s_Mutex;
        };
    }
}
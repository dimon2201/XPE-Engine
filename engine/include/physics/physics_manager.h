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
        class MainDispatcher;
    }

    namespace ecs
    {
        class Entity;
        class Scene;
    }

    namespace render
    {
        class Material;
    }

    namespace physics
    {
        using namespace physx;
        using namespace core;
        using namespace ecs;

        class PhysicsAllocator;
        class PhysicsErrorCallback;
        class PhysicsSimulationEventCallback;

        enum class eShapeType
        {
            NONE = 0,
            SPHERE = 1,
            CAPSULE = 2,
            BOX = 3,
            PLANE = 4
        };

        struct ENGINE_API sShapeDescriptor : public Object
        {
            sShapeDescriptor(const eShapeType& type)
            {
                Type = type;
            }

            eShapeType Type;
        };

        struct ENGINE_API sSphereShapeDescriptor : public sShapeDescriptor
        {
            sSphereShapeDescriptor() = delete;
            sSphereShapeDescriptor(f32 radius) : sShapeDescriptor(eShapeType::SPHERE), Radius(radius) {}

            f32 Radius;
        };

        struct ENGINE_API sCapsuleShapeDescriptor : public sShapeDescriptor
        {
            sCapsuleShapeDescriptor() = delete;
            sCapsuleShapeDescriptor(f32 radius, f32 halfHeight)
                : sShapeDescriptor(eShapeType::CAPSULE), Radius(radius), HalfHeight(halfHeight) {}

            f32 Radius;
            f32 HalfHeight;
        };

        struct ENGINE_API sBoxShapeDescriptor : public sShapeDescriptor
        {
            sBoxShapeDescriptor() = delete;
            sBoxShapeDescriptor(const glm::vec3& halfExtents)
                : sShapeDescriptor(eShapeType::BOX), HalfExtents(halfExtents) {}

            glm::vec3 HalfExtents;
        };

        struct ENGINE_API sPlaneShapeDescriptor : public sShapeDescriptor
        {
            sPlaneShapeDescriptor() : sShapeDescriptor(eShapeType::PLANE) {}
        };

        struct ENGINE_API sActor : public Object
        {
            enum class eActorType
            {
                NONE = 0,
                RIGID_STATIC = 1,
                RIGID_DYNAMIC = 2
            };

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

        struct ENGINE_API sScene : public Object
        {
            sScene(PxScene* scene) : Scene(scene)
            {
            }

            PxScene* Scene;
        };

        class ENGINE_API PhysicsManager final
        {

        public:
            static void Init(MainDispatcher* dispatcher);
            static void Free();

            static void EnableLoggingInfo(bool enable);
            static void EnableLoggingWarning(bool enable);
            static void EnableLoggingError(bool enable);

            static sActor* AddActor(
                Entity* entity,
                Scene* scene,
                const sActor::eActorType& actorType,
                sShapeDescriptor* shapeDesc,
                const glm::vec3& linearVelocity,
                f32 staticFriction,
                f32 dynamicFriction,
                f32 restitution,
                f32 contactOffset,
                f32 restOffset
            );
            static sScene* AddScene(Scene* scene);

        private:
            static bool EnableMemoryProfiling;
            static PhysicsAllocator* s_Allocator;
            static PhysicsErrorCallback* s_ErrorCallback;
            static PhysicsSimulationEventCallback* s_EventCallback;

            static PxFoundation* s_Foundation;
            static PxPhysics* s_Physics;
            static PxCpuDispatcher* s_Dispatcher;

            static std::unordered_map<string, sActor*>* s_Actors;
            static std::unordered_map<string, sScene*>* s_Scenes;
        };

    }

}
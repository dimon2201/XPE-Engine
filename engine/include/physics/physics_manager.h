#pragma once

#include <unordered_map>

#include <core/types.hpp>

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

        class PhysicsAllocator;
        class PhysicsErrorCallback;
        class PhysicsSimulationEventCallback;

        struct sActor : public core::Object
        {
            enum class eActorType
            {
                NONE = 0,
                RIGID_STATIC = 1,
                RIGID_DYNAMIC = 2
            };

            enum class eShapeType
            {
                NONE = 0,
                BOX = 1,
                PLANE = 2
            };

            sActor() = default;

            sActor(
                PxActor* actor,
                PxMaterial* material,
                PxShape* shape,
                const eActorType& actorType,
                const eShapeType& shapeType,
                const glm::vec3& linearVelocity,
                core::f32 staticFriction,
                core::f32 dynamicFriction,
                core::f32 restitution,
                core::f32 contactOffset,
                core::f32 restOffset
            ) :
                Actor(actor),
                Material(material),
                Shape(shape),
                ActorType(actorType),
                ShapeType(shapeType),
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
            eShapeType ShapeType = eShapeType::NONE;
            glm::vec3 LinearVelocity = glm::vec3(0.0f);
            core::f32 StaticFriction = 0.0f;
            core::f32 DynamicFriction = 0.0f;
            core::f32 Restitution = 0.0f;
            core::f32 ContactOffset = 0.0f;
            core::f32 RestOffset = 0.0f;
        };

        struct sScene : public core::Object
        {
            sScene(PxScene* scene) : Scene(scene)
            {
            }

            PxScene* Scene;
        };

        class ENGINE_API PhysicsManager final
        {

        public:
            static void Init(core::MainDispatcher* dispatcher);
            static void Free();

            static void EnableLoggingInfo(bool enable);
            static void EnableLoggingWarning(bool enable);
            static void EnableLoggingError(bool enable);

            static sActor* AddActor(ecs::Entity* entity, ecs::Scene* scene, const sActor::eActorType& actorType, const sActor::eShapeType& shapeType, const glm::vec3& linearVelocity, core::f32 staticFriction, core::f32 dynamicFriction, core::f32 restitution, core::f32 contactOffset, core::f32 restOffset);
            static sScene* AddScene(ecs::Scene* scene);

        private:
            static bool EnableMemoryProfiling;
            static PhysicsAllocator* s_Allocator;
            static PhysicsErrorCallback* s_ErrorCallback;
            static PhysicsSimulationEventCallback* s_EventCallback;

            static PxFoundation* s_Foundation;
            static PxPhysics* s_Physics;
            static PxCpuDispatcher* s_Dispatcher;

            static std::unordered_map<core::string, sActor*>* s_Actors;
            static std::unordered_map<core::string, sScene*>* s_Scenes;
        };

    }

}
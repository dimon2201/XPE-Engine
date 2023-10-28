#include <PxPhysicsAPI.h>

#include <physics/physics_system.h>
#include <physics/physics_manager.h>
#include <ecs/ecs.h>
#include <ecs/components.hpp>

namespace xpe
{
	namespace physics
	{
        glm::vec3 QuatToEuler(core::f32 w, core::f32 x, core::f32 y, core::f32 z)
        {
            glm::quat quaternion(w, x, y, z);

            return glm::eulerAngles(quaternion);
        }
     
        void PhysicsSystem::Update(ecs::Scene* scene, const core::Time& dt)
        {
            scene->PhysicsScene->Scene->simulate(dt.Millis());
            scene->PhysicsScene->Scene->fetchResults(true);

            scene->EachComponent<ecs::RigidBodyComponent>(
                []
                (ecs::RigidBodyComponent* component)
                {
                    ecs::Entity* entity = component->Entity;

                    PxTransform actorTransform;
                    
                    switch (component->Actor->Type)
                    {

                    case sActor::eType::RIGID_DYNAMIC:
                        actorTransform = ((PxRigidDynamic*)component->Actor->Actor)->getGlobalPose();
                        break;

                    }

                    ecs::ColliderComponent* collider = entity->GetComponent<ecs::ColliderComponent>();
                    if (collider == nullptr)
                    {
                        LogWarning("PhysicsSystem: Entity '" + entity->GetTag() + "' does not have ColliderComponent.");
                        return;
                    }

                    PxShape* shape = collider->Shape->Shape;

                    PxTransform shapeTransform = shape->getLocalPose();
                    glm::vec3 ShapeEuler = QuatToEuler(
                        actorTransform.q.w, actorTransform.q.x, actorTransform.q.y, actorTransform.q.z
                    );

                    entity->Transform.Position.x = actorTransform.p.x + shapeTransform.p.x;
                    entity->Transform.Position.y = actorTransform.p.y + shapeTransform.p.y;
                    entity->Transform.Position.z = actorTransform.p.z + shapeTransform.p.z;

                    entity->Transform.Rotation.x = ShapeEuler.x;
                    entity->Transform.Rotation.y = ShapeEuler.y;
                    entity->Transform.Rotation.z = ShapeEuler.z;

                    //shape.Collision->PosisionX.store(ATransform.p.x + STransform.p.x, std::memory_order_relaxed);
                    //shape.Collision->PosisionY.store(ATransform.p.y + STransform.p.y, std::memory_order_relaxed);
                    //shape.Collision->PosisionZ.store(ATransform.p.z + STransform.p.z, std::memory_order_relaxed);

                    //shape.Collision->RotationX.store(ShapeEuler.x, std::memory_order_relaxed);
                    //shape.Collision->RotationY.store(ShapeEuler.y, std::memory_order_relaxed);
                    //shape.Collision->RotationZ.store(ShapeEuler.z, std::memory_order_relaxed);
                }
            );
        }
	}
}
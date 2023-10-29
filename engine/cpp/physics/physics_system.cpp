#include <PxPhysicsAPI.h>

#include <physics/physics_system.h>
#include <physics/physics_manager.h>

#include <ecs/components.hpp>

namespace xpe
{
	namespace physics
	{
        glm::vec3 QuatToEuler(f32 w, f32 x, f32 y, f32 z)
        {
            glm::quat quaternion(w, x, y, z);

            return glm::eulerAngles(quaternion);
        }
     
        void PhysicsSystem::Update(Scene* scene, const Time& dt)
        {
            scene->PhysicsScene->Scene->simulate(1.0f / 60.0f);
            scene->PhysicsScene->Scene->fetchResults(true);

            scene->EachComponent<RigidBodyComponent>(
                []
                (RigidBodyComponent* component)
                {
                    Entity* entity = component->Entity;

                    PxTransform actorTransform;
                    
                    switch (component->ActorType)
                    {

                    case sActor::eActorType::RIGID_STATIC:
                        actorTransform = ((PxRigidStatic*)component->Actor)->getGlobalPose();
                        break;

                    case sActor::eActorType::RIGID_DYNAMIC:
                        actorTransform = ((PxRigidDynamic*)component->Actor)->getGlobalPose();
                        break;

                    }

                    PxShape* shape = component->Shape;

                    PxTransform shapeTransform = shape->getLocalPose();
                    glm::vec3 shapeEuler = QuatToEuler(
                        actorTransform.q.w, actorTransform.q.x, actorTransform.q.y, actorTransform.q.z
                    );

                    // Prevent concurrent access
                    static std::mutex mtx;
                    {
                        std::lock_guard<std::mutex> lock(mtx);

                        entity->Transform.Position.x = actorTransform.p.y;
                        entity->Transform.Position.y = actorTransform.p.x;
                        entity->Transform.Position.z = -actorTransform.p.z;

                        entity->Transform.Rotation.x = shapeEuler.y;
                        entity->Transform.Rotation.y = shapeEuler.x;
                        entity->Transform.Rotation.z = -shapeEuler.z;
                    }

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
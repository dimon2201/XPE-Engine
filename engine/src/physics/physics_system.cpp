#include <PxPhysicsAPI.h>

#include <physics/physics_system.hpp>
#include <physics/physics_manager.hpp>

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
     
        void cPhysicsSystem::Update(cScene* scene, const cTime& dt)
        {
            scene->PhysicsScene->simulate(1.0f / 60.0f);
            scene->PhysicsScene->fetchResults(true);

            auto components = scene->GetComponents<CTransform, CPhysicsActor>();
            for (auto [entity, transform, actor] : components.each())
            {
                PxTransform actorTransform;

                switch (actor.ActorType) {

                    case sActor::eActorType::RIGID_STATIC:
                        actorTransform = ((PxRigidStatic *) actor.Actor)->getGlobalPose();
                        break;

                    case sActor::eActorType::RIGID_DYNAMIC:
                        actorTransform = ((PxRigidDynamic *) actor.Actor)->getGlobalPose();
                        break;

                }

                PxShape *shape = actor.Shape;

                glm::vec3 shapeEuler = QuatToEuler(
                        actorTransform.q.w, actorTransform.q.x, actorTransform.q.y, actorTransform.q.z
                );

                // Prevent concurrent access
                {
                    std::lock_guard<std::mutex> lock(m_Mutex);
                    transform.Position = glm::vec3(actorTransform.p.y, actorTransform.p.x, -actorTransform.p.z);
                    transform.Rotation = glm::vec3(shapeEuler.y, shapeEuler.x, -shapeEuler.z);
                }
            }
        }
	}
}
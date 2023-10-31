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
     
        void cPhysicsSystem::Update(cScene* scene, const cTime& dt)
        {
            scene->PhysicsScene->simulate(1.0f / 60.0f);
            scene->PhysicsScene->fetchResults(true);

            scene->EachComponent<sCPhysicsActor>(
                [this]
                (sCPhysicsActor* component)
                {
                    cEntity* entity = component->Entity;

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

                    glm::vec3 shapeEuler = QuatToEuler(
                        actorTransform.q.w, actorTransform.q.x, actorTransform.q.y, actorTransform.q.z
                    );

                    // Prevent concurrent access
                    {
                        std::lock_guard<std::mutex> lock(m_Mutex);

                        entity->SetPosition(glm::vec3(actorTransform.p.y, actorTransform.p.x, -actorTransform.p.z));
                        entity->SetRotation(glm::vec3(shapeEuler.y, shapeEuler.x, -shapeEuler.z));
                    }
                }
            );
        }
	}
}
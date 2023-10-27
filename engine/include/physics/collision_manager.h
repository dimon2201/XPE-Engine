#pragma once
#include <physics/core/core.h>

namespace xpe {

	namespace physics {

		using namespace render;
		using namespace math;

		glm::vec3 ENGINE_API QuatToEuler(float w, float x, float y, float z); // temporary function created to convert quat to EulerXYZ

		class ENGINE_API CollisionSystem {
		public:

			// This method serves to create a dynamic actor. It is necessary to add the same one a little later to create a static one
			static Ref<PhysicDynamicActor> AddPhysicDynamicActor(glm::vec3 position, glm::vec3 linearVelocity = { 0.0f, 0.0f, 0.0f });

			// Reminder: it is necessary to make it so when creating you need to require a flag, what kind of physical object it will be. for simulation or a trigger 
			// This method creates a shape for the specified actor and adds it to PhysicDynamicActor->Shapes. (not completely finished, but it works)
			static PhysicShapeCollision* AddPhysicShape(Ref<PhysicDynamicActor>& actor, PhysicMaterial material, PhysicShapeData data, glm::vec3 halfExtents);

			// we need to implement AddPhysicShape for the other shapes
		};

	}
}
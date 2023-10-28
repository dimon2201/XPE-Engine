#pragma once

#include <ecs/ecs.h>
#include <core/types.hpp>

namespace xpe
{
	namespace physics
	{
		class PhysicsSystem : public ecs::System
		{

		public:
			void Update(ecs::Scene* scene, const core::Time& dt) override final;

		};
	}
}
#pragma once

namespace xpe
{
	namespace physics
	{
		class PhysicsSystem : public System
		{

		public:
			void Update(Scene* scene, const Time& dt) override final;

		};
	}
}
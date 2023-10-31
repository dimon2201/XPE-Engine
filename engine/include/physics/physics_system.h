#pragma once

namespace xpe
{
	namespace physics
	{
		class PhysicsSystem : public cSystem
		{
		public:
			void Update(cScene* scene, const Time& dt) override final;
		};
	}
}
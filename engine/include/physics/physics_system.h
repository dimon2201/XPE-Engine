#pragma once

namespace xpe
{
	namespace physics
	{
		class cPhysicsSystem : public cSystem
		{
		public:
			void Update(cScene* scene, const cTime& dt) override final;
		};
	}
}
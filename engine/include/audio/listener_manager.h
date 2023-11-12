#pragma once
namespace xpe {

	namespace audio {

		class ENGINE_API cListenerManager final
		{
		public:
			static void SetPosition(glm::vec3 position);
			static void SetVelocity(glm::vec3 velocity);

			static void SetOrientation(f32 Pitch, f32 Yaw, f32 Roll, glm::vec3 up);
	
			static void SetGain(f32 volume);			  

			static void SetDopplerFactor(f32 factor);
			static void SetSpeedOfSound(f32 seed);
		};
	}
}

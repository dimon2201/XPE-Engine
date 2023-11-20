#pragma once
namespace xpe {

	namespace audio {

		class ENGINE_API cListeningManager final
		{
		public:
			static void SetPosition(glm::vec3 position);
			static void SetVelocity(glm::vec3 velocity);

			static void SetOrientation(f32 pitch, f32 yaw, f32 roll, glm::vec3 up);
	
			static void SetGain(f32 volume);			  

			static void SetDopplerFactor(f32 factor);
			static void SetSpeedOfSound(f32 seed);
		};
	}
}

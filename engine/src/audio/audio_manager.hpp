#pragma once

namespace xpe {

	namespace audio {

		class ENGINE_API cAudioManager final
		{

		public:
			static void SetListenerPosition(glm::vec3 position);
			static void SetListenerVelocity(glm::vec3 velocity);

			static void SetListenerOrientation(f32 pitch, f32 yaw, f32 roll, glm::vec3 up);
			static void SetListenerGain(f32 volume);

			static void SetListenerDopplerFactor(f32 factor);
			static void SetListenerSpeedOfSound(f32 seed);

			static void VoiceRecord();

		private:
			static bool s_Recording;

			friend class cAudioSystem;
		};
	}
}

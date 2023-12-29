#include <audio/audio_manager.hpp>
#include <audio/core/context.hpp>

namespace xpe {

	namespace audio {

		bool cAudioManager::s_Recording = false;

		void cAudioManager::SetListenerPosition(glm::vec3 position)
		{
			context::SetListenerPosition(position);
		}

		void cAudioManager::SetListenerVelocity(glm::vec3 velocity)
		{
			context::SetListenerVelocity(velocity);
		}

		void cAudioManager::SetListenerOrientation(f32 pitch, f32 yaw, f32 roll, glm::vec3 up)
		{
			auto orientation = glm::quat(glm::vec3(-pitch, -yaw, roll));
			glm::vec3 front = glm::normalize(glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f)));

			context::SetListenerOrientation(front, up);
		}

		void cAudioManager::SetListenerGain(f32 volume)
		{
			context::SetListenerGain(volume);
		}

		void cAudioManager::SetListenerDopplerFactor(f32 factor)
		{
			context::SetListenerDopplerFactor(factor);
		}

		void cAudioManager::SetListenerSpeedOfSound(f32 speed)
		{
			context::SetListenerSpeedOfSound(speed);
		}

		void cAudioManager::VoiceRecord()
		{
			s_Recording = !s_Recording;

			if(s_Recording) {
				context::StartRecord();
			}
			else {
				context::StopRecord();
			}
		}
	}
}

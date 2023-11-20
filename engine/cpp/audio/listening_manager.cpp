#include <audio/listening_manager.h>

#include <audio/core/context.h>

namespace xpe {

	namespace audio {

		using namespace context;

		void cListeningManager::SetPosition(glm::vec3 position)
		{
			SetListenerPosition(position);
		}

		void cListeningManager::SetVelocity(glm::vec3 velocity)
		{
			SetListenerVelocity(velocity);
		}

		void cListeningManager::SetOrientation(f32 pitch, f32 yaw, f32 roll, glm::vec3 up)
		{
			auto orientation = glm::quat(glm::vec3(-pitch, -yaw, roll));
			glm::vec3 front = glm::normalize(glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f)));

			SetListenerOrientation(front, up);
		}

		void cListeningManager::SetGain(f32 volume)
		{
			SetListenerGain(volume);
		}

		void cListeningManager::SetDopplerFactor(f32 factor)
		{
			SetListenerDopplerFactor(factor);
		}

		void cListeningManager::SetSpeedOfSound(f32 speed)
		{
			SetListenerSpeedOfSound(speed);
		}
	}
}

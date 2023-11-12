#include <audio/listener_manager.h>

#include <audio/core/context.h>

namespace xpe {

	namespace audio {

		using namespace context;

		void cListenerManager::SetPosition(glm::vec3 position)
		{
			SetListenerPosition(position);
		}

		void cListenerManager::SetVelocity(glm::vec3 velocity)
		{
			SetListenerVelocity(velocity);
		}

		void cListenerManager::SetOrientation(f32 Pitch, f32 Yaw, f32 Roll, glm::vec3 up)
		{
			auto orientation = glm::quat(glm::vec3(-Pitch, -Yaw, Roll));
			glm::vec3 front = glm::normalize(glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f)));

			SetListenerOrientation(front, up);
		}

		void cListenerManager::SetGain(f32 volume)
		{
			SetListenerGain(volume);
		}

		void cListenerManager::SetDopplerFactor(f32 factor)
		{
			SetListenerDopplerFactor(factor);
		}

		void cListenerManager::SetSpeedOfSound(f32 speed)
		{
			SetListenerSpeedOfSound(speed);
		}
	}
}

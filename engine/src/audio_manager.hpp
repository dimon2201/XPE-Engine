#pragma once

#include <audio_context.hpp>

namespace xpe::ecs
{
	class cScene;
	class cEntity;
	class CAudio;
	class CStreamAudio;
}

namespace xpe
{
	namespace audio
	{
		class ENGINE_API MAudio final {

		public:
			static void Init();

			static void Free();

			static void Update(cScene *scene, float dt);

			static void SetListenerPosition(glm::vec3 position);

			static void SetListenerVelocity(glm::vec3 velocity);

			static void SetListenerOrientation(const glm::vec3& rotation, const glm::vec3& up);

			static void SetListenerGain(f32 volume);

			static void SetListenerDopplerFactor(f32 factor);

			static void SetListenerSpeedOfSound(f32 seed);

			static void VoiceRecord();

			static sAudioFile* Load(const char *filepath);

		private:
			static void UpdateVoices(cScene *scene);

			static void UpdateAudios(cScene *scene);

			static void UpdateStreamAudios(cScene *scene);

			static void AudioInit(CAudio &component);

			static void AudioInit(CStreamAudio &component);

			static void AudioSet(CAudio &component);

			static void AudioSet(CStreamAudio &component);

			static void AudioUpdate(CAudio &component);

			static void AudioUpdate(CStreamAudio &component);

			static void AudioStop(CAudio &component);

			static void AudioStop(CStreamAudio &component);

			static bool s_Recording;
			static vector<signed char>* s_VoiceData;

			//Test varriables to hear myself \/
			static u32 s_MySourceID;
			static vector<u32>* s_MyBuffersID;
		};
	}
}
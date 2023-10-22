#pragma once

#include <ecs/manager.h>
#include <ecs/components.hpp>

namespace xpe {

	namespace audio {

        using namespace core;
		using namespace ecs;

		class ENGINE_API AudioManager : public Manager
		{

		public:
            AudioManager();
			~AudioManager();

			void UpdateListener(Scene* scene);

			void Update(Scene* scene, const Time& dt) override final;

			void UpdateVoices(Scene* scene);
			void RecordVoice(VoiceComponent* component); //(todo) Bug: When you start voice recording, there is an unpleasant sound at first

		private:

			void VoiceInit(VoiceComponent* component);

			void AudioInit(AudioComponent* component);
			void AudioSet(AudioComponent* component);
			void AudioUpdate(AudioComponent* component);
			void AudioStop(AudioComponent* component);

			void UpdateAudios(Scene* scene);

			void AudioInit(StreamAudioComponent* component);
			void AudioSet(StreamAudioComponent* component);
			void AudioUpdate(StreamAudioComponent* component);
			void AudioStop(StreamAudioComponent* component);

			void UpdateStreamAudios(Scene* scene);
		};
	}
}

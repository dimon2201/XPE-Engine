#pragma once

#include <ecs/components.hpp>

namespace xpe {

	namespace audio {

        using namespace core;
		using namespace ecs;

		class ENGINE_API cAudioSystem : public cSystem
		{

		public:
            cAudioSystem();
			~cAudioSystem();

			void Update(cScene* scene, const cTime& dt) override final;

			void UpdateVoices(cScene* scene);
			void RecordVoice(sCVoice* component); //(todo) Bug: When you start voice recording, there is an unpleasant sound at first

		private:

			void VoiceInit(sCVoice* component);

			void AudioInit(sCAudio* component);
			void AudioSet(sCAudio* component);
			void AudioUpdate(sCAudio* component);
			void AudioStop(sCAudio* component);

			void UpdateAudios(cScene* scene);

			void AudioInit(sCStreamAudio* component);
			void AudioSet(sCStreamAudio* component);
			void AudioUpdate(sCStreamAudio* component);
			void AudioStop(sCStreamAudio* component);

			void UpdateStreamAudios(cScene* scene);
		};
	}
}

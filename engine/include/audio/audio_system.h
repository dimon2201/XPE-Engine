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

			void UpdateListener(cScene* scene);

			void Update(cScene* scene, const cTime& dt) override final;

			void UpdateVoices(cScene* scene);
			void RecordVoice(CVoice& component); //(todo) Bug: When you start voice recording, there is an unpleasant sound at first

		private:

			void VoiceInit(CVoice& component);

			void AudioInit(CAudio& component);
			void AudioSet(CAudio& component);
			void AudioUpdate(CAudio& component);
			void AudioStop(CAudio& component);

			void UpdateAudios(cScene* scene);

			void AudioInit(CStreamAudio& component);
			void AudioSet(CStreamAudio& component);
			void AudioUpdate(CStreamAudio& component);
			void AudioStop(CStreamAudio& component);

			void UpdateStreamAudios(cScene* scene);
		};
	}
}

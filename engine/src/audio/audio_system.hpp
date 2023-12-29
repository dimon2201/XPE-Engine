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

		private:
			void UpdateVoices(cScene* scene);
			void UpdateAudios(cScene* scene);
			void UpdateStreamAudios(cScene* scene);

			void VoiceRecord();

			void AudioInit(CAudio& component);
			void AudioInit(CStreamAudio& component);
			
			void AudioSet(CAudio& component);
			void AudioSet(CStreamAudio& component);
			
			void AudioUpdate(CAudio& component);
			void AudioUpdate(CStreamAudio& component);

			void AudioStop(CAudio& component);
			void AudioStop(CStreamAudio& component);

		private:
			vector<signed char> m_VoiceData;

			//Test varriables to hear myself \/
			u32 m_MySourceID;
			vector<u32> m_MyBuffersID;
		};
	}
}

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

			void AudioInit(sCAudio* component);
			void AudioInit(sCStreamAudio* component);
			
			void AudioSet(sCAudio* component);
			void AudioSet(sCStreamAudio* component);
			
			void AudioUpdate(sCAudio* component);
			void AudioUpdate(sCStreamAudio* component);

			void AudioStop(sCAudio* component);
			void AudioStop(sCStreamAudio* component);

		private:
			vector<signed char> m_VoiceData;

			//Test varriables to hear myself \/
			u32 m_MySourceID;
			vector<u32> m_MyBuffersID;
		};
	}
}

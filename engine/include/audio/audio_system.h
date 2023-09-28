#pragma once

namespace xpe {

	namespace ecs
	{
		class Scene;
		class AudioComponent;
		class StreamAudioComponent;
		class VoiceComponent;
	}

	namespace audio {

		using namespace xpe::ecs;
		using namespace xpe::core;

		class ENGINE_API AudioSystem : public core::Object
		{

		public:

			AudioSystem();
			~AudioSystem();

			void UpdateListener(Scene* scene);

			void Update(Scene* scene);

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

#include <audio/audio_system.h>
#include <audio/core/context.h>

namespace xpe {

	namespace audio {

        using namespace context;

        AudioSystem::AudioSystem()
		{
			context::InitAudio();
		}

        AudioSystem::~AudioSystem()
		{
			context::FreeAudio();
		}

		// Update listener's position, velocity and orientation
		void AudioSystem::UpdateListener(Scene* scene)
		{
			// todo : Maybe, if do it global, it will be better then now. And I dont need use EachComponent :|
			scene->EachComponent<ListenerComponent>([](ListenerComponent* component) {
				SetListenerPosition(*component->Position);
				SetListenerVelocity(component->Velocity);
				SetListenerOrientation(*component->Look, component->Up);
			});
		}

		// It's a cycle. multimedia playback and update audio's states
		void AudioSystem::Update(ecs::Scene* scene, const Time& dt)
		{
			//UpdateVoices(scene); //temporarily commented to not hearing myself
			UpdateAudios(scene);
			UpdateStreamAudios(scene);
		}

		void AudioSystem::UpdateVoices(Scene* scene)
		{
			scene->EachComponent<VoiceComponent>([this](VoiceComponent* component) {

				if (component->State != eAudioState::PLAYING) {
					VoiceInit(component);
				}

				RecordVoice(component);

				GetState(component->SourceID, component->State);
				if (component->Frames > 0) {
					UpdateBuffers(component->SourceID, component->BufferID.data(), component->Data.data(), component->Samples, SAMPLE_RATE);
				}
			});
		}

		void AudioSystem::RecordVoice(VoiceComponent* component)
		{
			GetProcessed(component->SourceID, &component->Frames);

			if (component->Frames > 0) {
				GetCaptureSamples(1, component->Samples);
				UploadSamplesToBuffer(component->Data.data(), component->Samples);
			}
		}

		void AudioSystem::VoiceInit(VoiceComponent* component)
		{
			GenSources(1, &component->SourceID);

			component->BufferID.reserve(component->NumBuffers);
			GenBuffers(component->NumBuffers, component->BufferID.data());

			component->Data.reserve(DATA_SIZE);
			StartRecord(component->SourceID, component->BufferID.data(), component->State, component->Data.data(), component->NumBuffers);
		}

		void AudioSystem::AudioInit(AudioComponent* component)
		{
			GenSources(1, &component->Source.Id);
			GenBuffers(1, &component->BufferID);
			
			UploadFileToBuffer(*component->File, component->BufferID);

			bindBuffers(component->Source.Id, component->BufferID);

			SetPosition(component->Source.Id, component->Source.Position);
			SetVelocity(component->Source.Id, component->Source.Position);

			SetGain(component->Source.Id, component->Source.Gain);
			SetPitch(component->Source.Id, component->Source.Pitch);

			SetRefDistance(component->Source.Id, component->Source.RefDistance);
			SetMaxDistance(component->Source.Id, component->Source.MaxDistance);

			SetRollOffFactor(component->Source.Id, component->Source.RollOffFactor);

			SetConeInnerAngle(component->Source.Id, component->Source.ConeInnerAngle);
			SetConeOuterAngle(component->Source.Id, component->Source.ConeOuterAngle);

			SetLooping(component->Source.Id, component->Source.Looping);
		}

		void AudioSystem::AudioSet(AudioComponent* component)
		{
			AudioInit(component);
			PlaySource(component->Source.Id);
		}

		void AudioSystem::AudioUpdate(AudioComponent* component)
		{
			GetState(component->Source.Id, component->Source.State);
		}

		void AudioSystem::AudioStop(AudioComponent* component)
		{
			StopAudio(component->Source.Id);
			UnbindBuffers(component->Source.Id);

			DeleteSources(1, &component->Source.Id);
			DeleteBuffers(1, &component->BufferID);

			component->Source.Id = 0;
		}

		void AudioSystem::UpdateAudios(Scene* scene)
		{
			scene->EachComponent<AudioComponent>([this](AudioComponent* component) {

				if (component->State == eAudioState::PLAYING) {
					AudioUpdate(component);
				}

				else if (component->State == eAudioState::INITIAL) {
					AudioSet(component);
					component->State = eAudioState::PLAYING;
				}

				else if (component->State == eAudioState::STOPPED) {
					AudioStop(component);
				}

			});
		}

		void AudioSystem::AudioInit(StreamAudioComponent* component)
		{
			GenSources(1, &component->Source.Id);

			component->BufferID = vector<u32>(component->NumBuffers);
			GenBuffers(component->NumBuffers, component->BufferID.data());

			component->Data = vector<short>(GetBufferSize(component->File->Info.channels, component->BufferSamples));

			SetPosition(component->Source.Id, component->Source.Position);
			SetVelocity(component->Source.Id, component->Source.Position);

			SetGain(component->Source.Id, component->Source.Gain);
			SetPitch(component->Source.Id, component->Source.Pitch);

			SetRefDistance(component->Source.Id, component->Source.RefDistance);
			SetMaxDistance(component->Source.Id, component->Source.MaxDistance);

			SetRollOffFactor(component->Source.Id, component->Source.RollOffFactor);

			SetConeInnerAngle(component->Source.Id, component->Source.ConeInnerAngle);
			SetConeOuterAngle(component->Source.Id, component->Source.ConeOuterAngle);
		}

		void AudioSystem::AudioSet(StreamAudioComponent* component)
		{
			AudioInit(component);

			SetCurrentFrame(component->File->File, component->CurrentFrame);

			for (int i = 0; i < component->NumBuffers; i++) {
				UpdateBuffer(*component->File, component->Source.Id, component->BufferID[i], component->Data.data(), component->BufferSamples, 0);
				component->CurrentFrame += component->BufferSamples;
			}

			PlaySource(component->Source.Id);
		}

		void AudioSystem::AudioUpdate(StreamAudioComponent* component)
		{
			s32 processed;

			GetProcessed(component->Source.Id, &processed);
			GetState(component->Source.Id, component->Source.State);

			if (GetError() == eAudioError::NONE) {

				for (s32 i = 0; i < component->NumBuffers && component->CurrentFrame < component->File->Info.frames && processed > 0; ++i) {

					SetCurrentFrame(component->File->File, component->CurrentFrame);
					component->CurrentFrame += component->BufferSamples;

					UpdateBuffer(*component->File, component->Source.Id, component->BufferID[i], component->Data.data(), component->BufferSamples, processed);
					
					--processed;
				}
			}

			else {
				LogError("Error checking music source state");
			}
		}

		void AudioSystem::AudioStop(StreamAudioComponent* component)
		{
			StopAudio(component->Source.Id);
			UnbindBuffers(component->Source.Id);

			DeleteSources(1, &component->Source.Id);
			DeleteBuffers(component->NumBuffers, component->BufferID.data());

			component->Source.Id = 0;
		}

		void AudioSystem::UpdateStreamAudios(Scene* scene)
		{
			// todo : Need to fix Looping
			scene->EachComponent<StreamAudioComponent>([this](StreamAudioComponent* component) {

				if (component->State == eAudioState::PLAYING) {
					AudioUpdate(component);
				}

				else if (component->State == eAudioState::INITIAL) {
					AudioSet(component);
					component->State = eAudioState::PLAYING;
				}

				else if (component->State == eAudioState::STOPPED) {
					AudioStop(component);
				}

			});
		}
	}
}
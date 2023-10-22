#include <audio/audio_manager.h>
#include <audio/core/context.h>

#include <ecs/scenes.hpp>

using namespace xpe::audio::context;

namespace xpe {

	namespace audio {

        AudioManager::AudioManager()
		{
			context::InitAudio();
		}

        AudioManager::~AudioManager()
		{
			context::FreeAudio();
		}

		// Update listener's position, velocity and orientation
		void AudioManager::UpdateListener(Scene* scene)
		{
			// todo : Maybe, if do it global, it will be better then now. And I dont need use EachComponent :|
			scene->EachComponent<ListenerComponent>([scene](ListenerComponent* component) {
				SetListenerPosition(*component->Position);
				SetListenerVelocity(component->Velocity);
				SetListenerOrientation(*component->Look, component->Up);
			});
		}

		// It's a cycle. multimedia playback and update audio's states
		void AudioManager::Update(ecs::Scene* scene, const Time& dt)
		{
			//UpdateVoices(scene); //temporarily commented to not hearing myself
			UpdateAudios(scene);
			UpdateStreamAudios(scene);
		}

		void AudioManager::UpdateVoices(Scene* scene)
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

		void AudioManager::RecordVoice(VoiceComponent* component)
		{
			GetProcessed(component->SourceID, &component->Frames);

			if (component->Frames > 0) {
				GetCaptureSamples(1, component->Samples);
				UploadSamplesToBuffer(component->Data.data(), component->Samples);
			}
		}

		void AudioManager::VoiceInit(VoiceComponent* component)
		{
			GenSources(1, &component->SourceID);

			component->BufferID.reserve(component->NumBuffers);
			GenBuffers(component->NumBuffers, component->BufferID.data());

			component->Data.reserve(DATA_SIZE);
			StartRecord(component->SourceID, component->BufferID.data(), component->State, component->Data.data(), component->NumBuffers);
		}

		void AudioManager::AudioInit(AudioComponent* component)
		{
			GenSources(1, &component->Source->SourceID);
			GenBuffers(1, &component->BufferID);
			
			UploadFileToBuffer(*component->File, component->BufferID);

			bindBuffers(component->Source->SourceID, component->BufferID);

			SetPosition(component->Source->SourceID, component->Source->Position);
			SetVelocity(component->Source->SourceID, component->Source->Position);

			SetGain(component->Source->SourceID, component->Source->Gain);
			SetPitch(component->Source->SourceID, component->Source->Pitch);

			SetRefDistance(component->Source->SourceID, component->Source->RefDistance);
			SetMaxDistance(component->Source->SourceID, component->Source->MaxDistance);

			SetRollOffFactor(component->Source->SourceID, component->Source->RollOffFactor);

			SetConeInnerAngle(component->Source->SourceID, component->Source->ConeInnerAngle);
			SetConeOuterAngle(component->Source->SourceID, component->Source->ConeOuterAngle);

			SetLooping(component->Source->SourceID, component->Source->Looping);
		}

		void AudioManager::AudioSet(AudioComponent* component)
		{
			AudioInit(component);
			PlaySource(component->Source->SourceID);
		}

		void AudioManager::AudioUpdate(AudioComponent* component)
		{
			GetState(component->Source->SourceID, component->Source->State);
		}

		void AudioManager::AudioStop(AudioComponent* component)
		{
			StopAudio(component->Source->SourceID);
			UnbindBuffers(component->Source->SourceID);

			DeleteSources(1, &component->Source->SourceID);
			DeleteBuffers(1, &component->BufferID);

			component->Source->SourceID = 0;
		}

		void AudioManager::UpdateAudios(Scene* scene)
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

		void AudioManager::AudioInit(StreamAudioComponent* component)
		{
			GenSources(1, &component->Source->SourceID);

			component->BufferID = vector<u32>(component->NumBuffers);
			GenBuffers(component->NumBuffers, component->BufferID.data());

			component->Data = vector<short>(GetBufferSize(component->File->Info.channels, component->BufferSamples));

			SetPosition(component->Source->SourceID, component->Source->Position);
			SetVelocity(component->Source->SourceID, component->Source->Position);

			SetGain(component->Source->SourceID, component->Source->Gain);
			SetPitch(component->Source->SourceID, component->Source->Pitch);

			SetRefDistance(component->Source->SourceID, component->Source->RefDistance);
			SetMaxDistance(component->Source->SourceID, component->Source->MaxDistance);

			SetRollOffFactor(component->Source->SourceID, component->Source->RollOffFactor);

			SetConeInnerAngle(component->Source->SourceID, component->Source->ConeInnerAngle);
			SetConeOuterAngle(component->Source->SourceID, component->Source->ConeOuterAngle);
		}

		void AudioManager::AudioSet(StreamAudioComponent* component)
		{
			AudioInit(component);

			SetCurrentFrame(component->File->File, component->CurrentFrame);

			for (int i = 0; i < component->NumBuffers; i++) {
				UpdateBuffer(*component->File, component->Source->SourceID, component->BufferID[i], component->Data.data(), component->BufferSamples, 0);
				component->CurrentFrame += component->BufferSamples;
			}

			PlaySource(component->Source->SourceID);
		}

		void AudioManager::AudioUpdate(StreamAudioComponent* component)
		{
			s32 processed;

			GetProcessed(component->Source->SourceID, &processed);
			GetState(component->Source->SourceID, component->Source->State);

			if (GetError() == eAudioError::NONE) {

				for (s32 i = 0; i < component->NumBuffers && component->CurrentFrame < component->File->Info.frames && processed > 0; ++i) {

					SetCurrentFrame(component->File->File, component->CurrentFrame);
					component->CurrentFrame += component->BufferSamples;

					UpdateBuffer(*component->File, component->Source->SourceID, component->BufferID[i], component->Data.data(), component->BufferSamples, processed);
					
					--processed;
				}
			}

			else {
				LogError("Error checking music source state");
			}
		}

		void AudioManager::AudioStop(StreamAudioComponent* component)
		{
			StopAudio(component->Source->SourceID);
			UnbindBuffers(component->Source->SourceID);

			DeleteSources(1, &component->Source->SourceID);
			DeleteBuffers(component->NumBuffers, component->BufferID.data());

			component->Source->SourceID = 0;
		}

		void AudioManager::UpdateStreamAudios(Scene* scene)
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
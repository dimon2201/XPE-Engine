#include <audio/system/audio_system.h>

#include <audio/core/openal_context.h>

#include <ecs/scenes.hpp>
#include <ecs/components.hpp>

using namespace xpe::audio::context;

namespace xpe {

	namespace audio {

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
			// (todo) Maybe, if do it global, it will be better then now. And I dont need use EachComponent :|
			scene->EachComponent<ListenerComponent>([scene](ListenerComponent* component) {
				SetListenerPosition(*component->Position);
				SetListenerVelocity(component->Velocity);
				SetListenerOrientation(*component->Look, component->Up);
			});
		}

		// It's a cycle. multimedia playback and update audio's states
		void AudioSystem::Update(ecs::Scene* scene)
		{
			//UpdateVoices(scene); //temporarily commented to not hearing myself
			UpdateAudios(scene);
			UpdateStreamAudios(scene);
		}

		void AudioSystem::UpdateVoices(Scene* scene)
		{
			scene->EachComponent<VoiceComponent>([this](VoiceComponent* component) {

				if (component->State != AUDIO_PLAYING) {
					VoiceInit(component);
				}

				RecordVoice(component);

				GetState(component->SourceID, &component->State);
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

		void AudioSystem::AudioSet(AudioComponent* component)
		{
			AudioInit(component);
			PlaySource(component->Source->SourceID, component->Source->State);
		}

		void AudioSystem::AudioUpdate(AudioComponent* component)
		{
			GetState(component->Source->SourceID, &component->Source->State);
		}

		void AudioSystem::AudioStop(AudioComponent* component)
		{
			StopAudio(component->Source->SourceID);
			UnbindBuffers(component->Source->SourceID);

			DeleteSources(1, &component->Source->SourceID);
			DeleteBuffers(1, &component->BufferID);

			component->Source->SourceID = 0;
		}

		void AudioSystem::UpdateAudios(Scene* scene)
		{
			scene->EachComponent<AudioComponent>([this](AudioComponent* component) {
				if (component->Status == AUDIO_PLAYING) {
					AudioUpdate(component);
				}
				else if (component->Status == AUDIO_INITIAL) {
					AudioSet(component);
					component->Status = AUDIO_PLAYING;
				}
				else if (component->Status == AUDIO_STOPPED) {
					AudioStop(component);
				}
			});
		}

		void AudioSystem::AudioInit(StreamAudioComponent* component)
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

		void AudioSystem::AudioSet(StreamAudioComponent* component)
		{
			AudioInit(component);

			SetCurrentFrame(component->File->File, component->CurrentFrame);

			for (int i = 0; i < component->NumBuffers; i++) {
				UpdateBuffer(*component->File, component->Source->SourceID, component->BufferID[i], component->Data.data(), component->BufferSamples, 0);
				component->CurrentFrame += component->BufferSamples;
			}

			PlaySource(component->Source->SourceID, component->Source->State);
		}

		void AudioSystem::AudioUpdate(StreamAudioComponent* component)
		{
			s32 processed;

			GetProcessed(component->Source->SourceID, &processed);
			GetState(component->Source->SourceID, &component->Source->State);

			if (alGetError() == AL_NO_ERROR) {

				for (s32 i = 0; i < component->NumBuffers && component->CurrentFrame < component->File->Info.frames && processed > 0; ++i) {

					SetCurrentFrame(component->File->File, component->CurrentFrame);
					component->CurrentFrame += component->BufferSamples;

					UpdateBuffer(*component->File, component->Source->SourceID, component->BufferID[i], component->Data.data(), component->BufferSamples, processed);
					
					--processed;
				}
			}
			else {
				LogInfo("Error checking music source state");
			}
		}

		void AudioSystem::AudioStop(StreamAudioComponent* component)
		{
			StopAudio(component->Source->SourceID);
			UnbindBuffers(component->Source->SourceID);

			DeleteSources(1, &component->Source->SourceID);
			DeleteBuffers(component->NumBuffers, component->BufferID.data());

			component->Source->SourceID = 0;
		}

		void AudioSystem::UpdateStreamAudios(Scene* scene)
		{
			// Need fix Looping 
			scene->EachComponent<StreamAudioComponent>([this](StreamAudioComponent* component) {
				if (component->Status == AUDIO_PLAYING) {
					AudioUpdate(component);
				}
				else if (component->Status == AUDIO_INITIAL) {
					AudioSet(component);
					component->Status = AUDIO_PLAYING;
				}
				else if(component->Status == AUDIO_STOPPED) {
					AudioStop(component);
				}
			});
		}
	}
}
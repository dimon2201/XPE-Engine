#include <audio/audio_system.h>
#include <audio/core/context.h>

namespace xpe {

	namespace audio {

        using namespace context;

        cAudioSystem::cAudioSystem()
		{
			context::InitAudio();
		}

        cAudioSystem::~cAudioSystem()
		{
			context::FreeAudio();
		}

		// Update listener's position, velocity and orientation
		void cAudioSystem::UpdateListener(cScene* scene)
		{
			// todo : Maybe, if do it global, it will be better then now. And I dont need use ForEach :|
            auto components = scene->GetComponents<CListener>();
            for (auto [entity, listener] : components.each())
            {
                SetListenerPosition(*listener.Position);
                SetListenerVelocity(listener.Velocity);
                SetListenerOrientation(*listener.Look, listener.Up);
            }
		}

		// It's a cycle. multimedia playback and update audio's states
		void cAudioSystem::Update(ecs::cScene* scene, const cTime& dt)
		{
			//UpdateVoices(scene); //temporarily commented to not hearing myself
			UpdateAudios(scene);
			UpdateStreamAudios(scene);
		}

		void cAudioSystem::UpdateVoices(cScene* scene)
		{
            auto components = scene->GetComponents<CVoice>();
            for (auto [entity, voice] : components.each())
            {
                if (voice.State != eAudioState::PLAYING) {
                    VoiceInit(voice);
                }

                RecordVoice(voice);

                GetState(voice.SourceID, voice.State);
                if (voice.Frames > 0) {
                    UpdateBuffers(voice.SourceID, voice.BufferID.data(), voice.Data.data(), voice.Samples, k_SampleRate);
                }
            }
		}

		void cAudioSystem::RecordVoice(CVoice& component)
		{
			GetProcessed(component.SourceID, &component.Frames);

			if (component.Frames > 0) {
				GetCaptureSamples(1, component.Samples);
				UploadSamplesToBuffer(component.Data.data(), component.Samples);
			}
		}

		void cAudioSystem::VoiceInit(CVoice& component)
		{
			GenSources(1, &component.SourceID);

			component.BufferID.reserve(component.NumBuffers);
			GenBuffers(component.NumBuffers, component.BufferID.data());

			component.Data.reserve(k_DataSize);
			StartRecord(component.SourceID, component.BufferID.data(), component.State, component.Data.data(), component.NumBuffers);
		}

		void cAudioSystem::AudioInit(CAudio& component)
		{
			GenSources(1, &component.Source.Id);
			GenBuffers(1, &component.BufferID);
			
			UploadFileToBuffer(*component.File, component.BufferID);

			bindBuffers(component.Source.Id, component.BufferID);

			SetPosition(component.Source.Id, component.Source.Position);
			SetVelocity(component.Source.Id, component.Source.Position);

			SetGain(component.Source.Id, component.Source.Gain);
			SetPitch(component.Source.Id, component.Source.Pitch);

			SetRefDistance(component.Source.Id, component.Source.RefDistance);
			SetMaxDistance(component.Source.Id, component.Source.MaxDistance);

			SetRollOffFactor(component.Source.Id, component.Source.RollOffFactor);

			SetConeInnerAngle(component.Source.Id, component.Source.ConeInnerAngle);
			SetConeOuterAngle(component.Source.Id, component.Source.ConeOuterAngle);

			SetLooping(component.Source.Id, component.Source.Looping);
		}

		void cAudioSystem::AudioSet(CAudio& component)
		{
			AudioInit(component);
			PlaySource(component.Source.Id);
		}

		void cAudioSystem::AudioUpdate(CAudio& component)
		{
			GetState(component.Source.Id, component.Source.State);
		}

		void cAudioSystem::AudioStop(CAudio& component)
		{
			StopAudio(component.Source.Id);
			UnbindBuffers(component.Source.Id);

			DeleteSources(1, &component.Source.Id);
			DeleteBuffers(1, &component.BufferID);

			component.Source.Id = 0;
		}

		void cAudioSystem::UpdateAudios(cScene* scene)
		{
            auto components = scene->GetComponents<CAudio>();
            for (auto [entity, audio] : components.each())
            {
                if (audio.State == eAudioState::PLAYING) {
                    AudioUpdate(audio);
                } else if (audio.State == eAudioState::INITIAL) {
                    AudioSet(audio);
                    audio.State = eAudioState::PLAYING;
                } else if (audio.State == eAudioState::STOPPED) {
                    AudioStop(audio);
                }
            }
		}

		void cAudioSystem::AudioInit(CStreamAudio& component)
		{
			GenSources(1, &component.Source.Id);

			component.BufferID = vector<u32>(component.NumBuffers);
			GenBuffers(component.NumBuffers, component.BufferID.data());

			component.Data = vector<short>(GetBufferSize(component.File->Info.channels, component.BufferSamples));

			SetPosition(component.Source.Id, component.Source.Position);
			SetVelocity(component.Source.Id, component.Source.Position);

			SetGain(component.Source.Id, component.Source.Gain);
			SetPitch(component.Source.Id, component.Source.Pitch);

			SetRefDistance(component.Source.Id, component.Source.RefDistance);
			SetMaxDistance(component.Source.Id, component.Source.MaxDistance);

			SetRollOffFactor(component.Source.Id, component.Source.RollOffFactor);

			SetConeInnerAngle(component.Source.Id, component.Source.ConeInnerAngle);
			SetConeOuterAngle(component.Source.Id, component.Source.ConeOuterAngle);
		}

		void cAudioSystem::AudioSet(CStreamAudio& component)
		{
			AudioInit(component);

			SetCurrentFrame(component.File->File, component.CurrentFrame);

			for (int i = 0; i < component.NumBuffers; i++) {
				UpdateBuffer(*component.File, component.Source.Id, component.BufferID[i], component.Data.data(), component.BufferSamples, 0);
				component.CurrentFrame += component.BufferSamples;
			}

			PlaySource(component.Source.Id);
		}

		void cAudioSystem::AudioUpdate(CStreamAudio& component)
		{
			s32 processed;

			GetProcessed(component.Source.Id, &processed);
			GetState(component.Source.Id, component.Source.State);

			if (GetError() == eAudioError::NONE) {

				for (s32 i = 0; i < component.NumBuffers && component.CurrentFrame < component.File->Info.frames && processed > 0; ++i) {

					SetCurrentFrame(component.File->File, component.CurrentFrame);
					component.CurrentFrame += component.BufferSamples;

					UpdateBuffer(*component.File, component.Source.Id, component.BufferID[i], component.Data.data(), component.BufferSamples, processed);
					
					--processed;
				}
			}

			else {
				LogError("Error checking music source state");
			}
		}

		void cAudioSystem::AudioStop(CStreamAudio& component)
		{
			StopAudio(component.Source.Id);
			UnbindBuffers(component.Source.Id);

			DeleteSources(1, &component.Source.Id);
			DeleteBuffers(component.NumBuffers, component.BufferID.data());

			component.Source.Id = 0;
		}

		void cAudioSystem::UpdateStreamAudios(cScene* scene)
		{
			// todo : Need to fix Looping
            auto components = scene->GetComponents<CStreamAudio>();
            for (auto [entity, audio] : components.each())
            {
                if (audio.State == eAudioState::PLAYING) {
                    AudioUpdate(audio);
                } else if (audio.State == eAudioState::INITIAL) {
                    AudioSet(audio);
                    audio.State = eAudioState::PLAYING;
                } else if (audio.State == eAudioState::STOPPED) {
                    AudioStop(audio);
                }
            }
		}
	}
}
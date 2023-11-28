#include <audio/audio_system.h>
#include <audio/core/context.h>
#include <audio/audio_manager.h>

namespace xpe {

	namespace audio {

        using namespace context;

        cAudioSystem::cAudioSystem()
		{
			context::InitAudio();
			m_VoiceData = vector<signed char>(k_CaptureBufferSize * k_NumCaptureBuffers, 0);
			 
			//Test varriables to hear myself \/
			GenSources(1, &m_MySourceID);

			m_MyBuffersID = vector<u32>(k_NumCaptureBuffers);
			GenBuffers(k_NumCaptureBuffers, m_MyBuffersID.data());

			AddBuffer(m_MySourceID, m_MyBuffersID.data(), m_VoiceData.data(),
				k_CaptureBufferSize, k_CaptureFrequency, k_NumCaptureBuffers);

			PlaySource(m_MySourceID);
		}

        cAudioSystem::~cAudioSystem()
		{
			context::FreeAudio();
		}

		void cAudioSystem::Update(ecs::cScene* scene, const cTime& dt)
		{
			if (cAudioManager::s_Recording) {
				VoiceRecord();
			}

			UpdateVoices(scene);
			UpdateAudios(scene);
			UpdateStreamAudios(scene);
		}

		void cAudioSystem::UpdateVoices(cScene* scene)
		{
			// For testing voice recorder
			{
				eAudioState State;
				s32 Processed;

				GetState(m_MySourceID, State);
				GetProcessed(m_MySourceID, &Processed);

				if(Processed) {

					UpdateBuffers(m_MySourceID, m_MyBuffersID.data(), m_VoiceData.data(),
						k_CaptureBufferSize, k_CaptureFrequency);

					if (State != eAudioState::PLAYING) {
						PlaySource(m_MySourceID);
					}
				}
			}

			// There is no sense in doing this now, so I'll leave it for later
			//scene->ForEach<sCVoice>([this](sCVoice *component) {
			//		//Voices update 
            //});
		}

		void cAudioSystem::UpdateAudios(cScene* scene)
		{
			scene->ForEach<sCAudio>([this](sCAudio* component) {

				if (component->State == eAudioState::PLAYING) {
					AudioUpdate(component);
				}

				else if (component->State == eAudioState::INITIAL) {
					AudioSet(component);
				}

				else if (component->State == eAudioState::STOPPED) {
					AudioStop(component);
				}

			});
		}

		void cAudioSystem::UpdateStreamAudios(cScene* scene)
		{
            scene->ForEach<sCStreamAudio>([this](sCStreamAudio*component) {

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

		void cAudioSystem::VoiceRecord()
		{
			s32 samples;
			GetCaptureSamples(1, samples);

			if(samples > k_CaptureBufferSize) {
				UploadSamplesToBuffer(m_VoiceData.data(), k_CaptureBufferSize);
			}
		}
		
		void cAudioSystem::AudioInit(sCAudio* component)
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

		void cAudioSystem::AudioInit(sCStreamAudio* component)
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

		void cAudioSystem::AudioSet(sCAudio* component)
		{
			AudioInit(component);
			PlaySource(component->Source.Id);
			GetState(component->Source.Id, component->State);
		}

		void cAudioSystem::AudioSet(sCStreamAudio* component)
		{
			AudioInit(component);

			SetCurrentFrame(component->File->File, component->CurrentFrame);

			for (int i = 0; i < component->NumBuffers; i++) {
				UpdateBuffer(*component->File, component->Source.Id, component->BufferID[i], component->Data.data(), component->BufferSamples, 0);
				component->CurrentFrame += component->BufferSamples;
			}

			PlaySource(component->Source.Id);

			component->State = eAudioState::PLAYING;
		}

		void cAudioSystem::AudioUpdate(sCAudio* component)
		{
			GetState(component->Source.Id, component->State);
		}

		void cAudioSystem::AudioUpdate(sCStreamAudio* component)
		{
			s32 processed;

			GetProcessed(component->Source.Id, &processed);
			GetState(component->Source.Id, component->State);

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

		void cAudioSystem::AudioStop(sCAudio* component)
		{
			StopSource(component->Source.Id);
			UnbindBuffers(component->Source.Id);

			DeleteSources(1, &component->Source.Id);
			DeleteBuffers(1, &component->BufferID);

			component->Source.Id = 0;

			component->State = eAudioState::PAUSED; // temporarily
		}

		void cAudioSystem::AudioStop(sCStreamAudio* component)
		{
			StopSource(component->Source.Id);
			UnbindBuffers(component->Source.Id);

			DeleteSources(1, &component->Source.Id);
			DeleteBuffers(component->NumBuffers, component->BufferID.data());

			component->Source.Id = 0;

			component->State = eAudioState::PAUSED; // temporarily
		}
	}
}
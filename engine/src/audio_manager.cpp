#include <audio_manager.hpp>
#include <components.hpp>

namespace xpe
{
	namespace audio
	{
		bool MAudio::s_Recording = false;
		vector<signed char>* MAudio::s_VoiceData = nullptr;
		u32 MAudio::s_MySourceID = 0;
		vector<u32>* MAudio::s_MyBuffersID = nullptr;

		static unordered_map<string, sAudioFile*>* s_AudioFiles = nullptr;

		using namespace context;

		void MAudio::Init()
		{
			Profile();

			context::InitAudio();
			s_AudioFiles = new unordered_map<string, sAudioFile*>();

			s_VoiceData = new vector<signed char>(k_CaptureBufferSize * k_NumCaptureBuffers, 0);

			//Test varriables to hear myself \/
			GenSources(1, &s_MySourceID);

			s_MyBuffersID = new vector<u32>(k_NumCaptureBuffers);
			GenBuffers(k_NumCaptureBuffers, s_MyBuffersID->data());

			AddBuffer(s_MySourceID, s_MyBuffersID->data(), s_VoiceData->data(),
					  k_CaptureBufferSize, k_CaptureFrequency, k_NumCaptureBuffers);

			PlaySource(s_MySourceID);
		}

		void MAudio::Free()
		{
			Profile();

			DeleteSources(1, &s_MySourceID);
			DeleteBuffers(k_NumCaptureBuffers, s_MyBuffersID->data());
			for (auto& audioFile : *s_AudioFiles)
			{
				delete audioFile.second;
			}
			delete s_AudioFiles;
			delete s_VoiceData;
            delete s_MyBuffersID;
			context::FreeAudio();
		}

		void MAudio::Update(ecs::cScene* scene, float dt)
		{
			Profile();

			UpdateVoices(scene);
			UpdateAudios(scene);
			UpdateStreamAudios(scene);
		}

		void MAudio::SetListenerPosition(glm::vec3 position)
		{
			context::SetListenerPosition(position);
		}

		void MAudio::SetListenerVelocity(glm::vec3 velocity)
		{
			context::SetListenerVelocity(velocity);
		}

		void MAudio::SetListenerOrientation(const glm::vec3& rotation, const glm::vec3& up)
		{
			auto orientation = glm::quat(glm::vec3(-rotation.x, -rotation.y, rotation.z));
			glm::vec3 front = glm::normalize(glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f)));

			context::SetListenerOrientation(front, up);
		}

		void MAudio::SetListenerGain(f32 volume)
		{
			context::SetListenerGain(volume);
		}

		void MAudio::SetListenerDopplerFactor(f32 factor)
		{
			context::SetListenerDopplerFactor(factor);
		}

		void MAudio::SetListenerSpeedOfSound(f32 speed)
		{
			context::SetListenerSpeedOfSound(speed);
		}

		void MAudio::VoiceRecord()
		{
			s_Recording = !s_Recording;

			if (s_Recording)
			{
				context::StartRecord();
			}
			else
			{
				context::StopRecord();
			}
		}

		void MAudio::UpdateVoices(cScene* scene)
		{
			Profile();

			// For testing voice recorder
			{
				eAudioState State;
				s32 Processed;

				GetState(s_MySourceID, State);
				GetProcessed(s_MySourceID, &Processed);

				if (Processed)
				{
					UpdateBuffers(s_MySourceID, s_MyBuffersID->data(), s_VoiceData->data(), k_CaptureBufferSize, k_CaptureFrequency);
					if (State != eAudioState::PLAYING)
					{
						PlaySource(s_MySourceID);
					}
				}
			}
		}

		void MAudio::UpdateAudios(cScene* scene)
		{
			Profile();

			auto components = scene->GetComponents<CAudio, CTransform>();
			for (auto [entity, audio, transform] : components.each())
			{
				if (audio.State == eAudioState::PLAYING)
				{
					AudioUpdate(audio);
					SetPosition(audio.Source.Id, transform.Position);
				}
				else if (audio.State == eAudioState::INITIAL)
				{
					AudioInit(audio);
					AudioSet(audio);
					SetPosition(audio.Source.Id, transform.Position);
				}
				else if (audio.State == eAudioState::STOPPED)
				{
					AudioStop(audio);
				}
			}
		}

		void MAudio::UpdateStreamAudios(cScene* scene)
		{
			Profile();

			auto components = scene->GetComponents<CStreamAudio, CTransform>();
			for (auto [entity, audio, transform] : components.each())
			{
				if (audio.State == eAudioState::PLAYING)
				{
					AudioUpdate(audio);
					SetPosition(audio.Source.Id, transform.Position);
				}
				else if (audio.State == eAudioState::INITIAL)
				{
					AudioInit(audio);
					AudioSet(audio);
					SetPosition(audio.Source.Id, transform.Position);
				}
				else if (audio.State == eAudioState::STOPPED)
				{
					AudioStop(audio);
				}
			}
		}

		// TODO : do we need this?
//		void MAudio::VoiceRecord()
//		{
//			s32 samples;
//			GetCaptureSamples(1, samples);
//			if (samples >= k_CaptureBufferSize)
//			{
//				UploadSamplesToBuffer(s_VoiceData.data(), k_CaptureBufferSize);
//			}
//		}

		void MAudio::AudioInit(CAudio& component)
		{
			GenSources(1, &component.Source.Id);
			GenBuffers(1, &component.BufferID);

			UploadFileToBuffer(component.File, component.BufferID);

			BindBuffers(component.Source.Id, component.BufferID);

			SetGain(component.Source.Id, component.Source.Gain);
			SetPitch(component.Source.Id, component.Source.Pitch);

			SetRefDistance(component.Source.Id, component.Source.RefDistance);
			SetMaxDistance(component.Source.Id, component.Source.MaxDistance);

			SetRollOffFactor(component.Source.Id, component.Source.RollOffFactor);

			SetConeInnerAngle(component.Source.Id, component.Source.ConeInnerAngle);
			SetConeOuterAngle(component.Source.Id, component.Source.ConeOuterAngle);

			SetLooping(component.Source.Id, component.Source.Looping);
		}

		void MAudio::AudioInit(CStreamAudio& component)
		{
			GenSources(1, &component.Source.Id);

			component.BufferID = vector<u32>(component.NumBuffers);
			GenBuffers(component.NumBuffers, component.BufferID.data());

			component.Data = vector<short>(GetBufferSize(component.File->Info.channels, component.BufferSamples));

			SetGain(component.Source.Id, component.Source.Gain);
			SetPitch(component.Source.Id, component.Source.Pitch);

			SetRefDistance(component.Source.Id, component.Source.RefDistance);
			SetMaxDistance(component.Source.Id, component.Source.MaxDistance);

			SetRollOffFactor(component.Source.Id, component.Source.RollOffFactor);

			SetConeInnerAngle(component.Source.Id, component.Source.ConeInnerAngle);
			SetConeOuterAngle(component.Source.Id, component.Source.ConeOuterAngle);
		}

		void MAudio::AudioSet(CAudio& component)
		{
			PlaySource(component.Source.Id);
			component.State = eAudioState::PLAYING;
		}

		void MAudio::AudioSet(CStreamAudio& component)
		{
			for (int i = 0; i < component.NumBuffers; i++)
			{
				SetCurrentFrame(component.File->File, component.CurrentFrame);
				component.CurrentFrame += component.BufferSamples;
				UpdateBuffer(component.File, component.Source.Id, component.BufferID[i], component.Data.data(), component.BufferSamples, false);
			}

			PlaySource(component.Source.Id);
			GetState(component.Source.Id, component.State);
		}

		void MAudio::AudioUpdate(CAudio& component)
		{
			GetState(component.Source.Id, component.State);
		}

		void MAudio::AudioUpdate(CStreamAudio& component)
		{
			s32 processed = 0;
			GetProcessed(component.Source.Id, &processed);
			GetState(component.Source.Id, component.State);

			if (GetError() == eAudioError::NONE)
			{
				if (component.State == eAudioState::STOPPED && component.CurrentFrame < component.File->Info.frames)
				{
					//Recreating audio
					AudioStop(component);
					AudioInit(component);
					AudioSet(component);
				}
				else
				{
					for (s32 i = 0; component.CurrentFrame < component.File->Info.frames && processed > 0; ++i)
					{
						SetCurrentFrame(component.File->File, component.CurrentFrame);
						component.CurrentFrame += component.BufferSamples;
						UpdateBuffer(component.File, component.Source.Id, component.BufferID[i], component.Data.data(), component.BufferSamples, true);
						--processed;
					}
				}
			}
			else
			{
				LogError("Error checking music source state");
			}
		}

		void MAudio::AudioStop(CAudio& component)
		{
			StopSource(component.Source.Id);
			UnbindBuffers(component.Source.Id);

			DeleteSources(1, &component.Source.Id);
			DeleteBuffers(1, &component.BufferID);

			component.Source.Id = 0;
			component.State = eAudioState::PAUSED;
		}

		void MAudio::AudioStop(CStreamAudio& component)
		{
			StopSource(component.Source.Id);

			DeleteSources(1, &component.Source.Id);
			DeleteBuffers(component.NumBuffers, component.BufferID.data());

			component.Source.Id = 0;
			component.State = eAudioState::PAUSED;
		}

		sAudioFile* MAudio::Load(const char* filepath)
		{
			Profile();

			if (MFile::NotExists(filepath))
			{
				LogError("Audio file not found in {}", filepath);
				return nullptr;
			}

			sAudioFile* audioFile = new sAudioFile();

			audioFile->File = sf_open(filepath, SFM_READ, &audioFile->Info);
			if (!audioFile->File)
			{
				LogError("Unable to open audio file {}", filepath);
			}

			audioFile->Info.format = context::GetFormat(*audioFile, audioFile->Info.channels);
			s_AudioFiles->insert({ filepath, audioFile });
			return audioFile;
		}
	}
}

#include <audio_loader.h>
#include <audio/core/context.h>

namespace xpe {

	namespace res {

        static unordered_map<string, sAudioFile*>* s_AudioFiles = nullptr;

        void AudioLoader::Init()
        {
            s_AudioFiles = new unordered_map<string, sAudioFile*>();
        }

        void AudioLoader::Free()
        {
            for (auto& audioFile : *s_AudioFiles) {
                delete audioFile.second;
            }
            delete s_AudioFiles;
        }

        sAudioFile* AudioLoader::Load(const char* filepath)
		{
            sAudioFile* audioFile = new sAudioFile();

            audioFile->File = sf_open(filepath, SFM_READ, &audioFile->Info);
			if (!audioFile->File) {
				LogError("Unable to open audio file {}", filepath);
			}

            audioFile->Info.format = context::GetFormat(*audioFile, audioFile->Info.channels);
            s_AudioFiles->insert({ filepath, audioFile });
			return audioFile;
		}

	}
}
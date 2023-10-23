#include <audio_loader.h>
#include <audio/core/context.h>

namespace xpe {

	namespace res {

		Ref<AudioFile> AudioLoader::Load(const char* filepath)
		{
            if (m_Map.find(filepath) != m_Map.end()) {
                Ref<AudioFile> audioRef;
                audioRef.Create(*m_Map[filepath]);
                return audioRef;
            }

			Ref<AudioFile> audioRef;
            audioRef.Create();

            audioRef->File = sf_open(filepath, SFM_READ, &audioRef->Info);
			if (!audioRef->File) {
				LogError("Unable to open file {}", filepath);
			}

            audioRef->Info.format = context::GetFormat(*audioRef, audioRef->Info.channels);

            m_Map.insert({ filepath, audioRef });
			return audioRef;
		}

	}
}
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

			Ref<AudioFile> fileRef;
            fileRef.Create(AudioFile());
            m_Map.insert({ filepath, fileRef });

            fileRef->File = sf_open(filepath, SFM_READ, &fileRef->Info);
			if (!fileRef->File) {
				LogError("Unable to open file {}", filepath);
			}

			fileRef->Info.format = context::GetFormat(*fileRef, fileRef->Info.channels);

			return fileRef;
		}

	}
}
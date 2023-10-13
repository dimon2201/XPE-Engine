#include <audio_loader.h>
#include <audio/core/context.h>

namespace xpe {

	namespace res {

		Ref<AudioFile> AudioLoader::Load(const char* filepath)
		{
            if (m_Storage->Has(filepath)) {
                Ref<AudioFile> audioFileRef;
                audioFileRef.Create(*m_Storage->Get(filepath));
                return audioFileRef;
            }

			Ref<AudioFile> fileRef = m_Storage->Add(filepath, AudioFile());
			AudioFile& file = *fileRef;

			file.File = sf_open(filepath, SFM_READ, &file.Info);
			if (!fileRef->File) {
				LogError("Unable to open file {}", filepath);
			}

			file.Info.format = context::GetFormat(file, file.Info.channels);

			return fileRef;
		}

	}
}
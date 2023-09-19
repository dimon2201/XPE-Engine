#include <stream_audio_loader.h>

namespace xpe {

	namespace res {

		Ref<AudioFile> AudioLoader::Load(const char* filepath)
		{
			Ref<AudioFile> fileRef = m_Storage->Add(filepath, AudioFile());
			AudioFile& file = *fileRef;

			file.File = sf_open(filepath, SFM_READ, &file.Info);
			if (!fileRef->File) {
				LogInfo("Ñould not open provided audio file");
			}

			file.Info.format = xpe::audio::context::GetFormat(file, file.Info.channels);

			return fileRef;
		}
	}
}
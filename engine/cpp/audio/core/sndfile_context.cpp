#include <audio/core/sndfile_context.h>

namespace xpe {

	namespace audio {

		AudioFile::~AudioFile()
		{
			sf_close(File);
		}

		namespace context {

			void SetCurrentFrame(SNDFILE* file, s64 frame)
			{
				sf_seek(file, frame, SEEK_SET);
			}

			s64 ReadChunk(SNDFILE* m_File, short* data, s64 num_frames)
			{
				return sf_readf_short(m_File, data, num_frames);
			}

			bool isAmbisonic(const AudioFile& file)
			{
				return sf_command(file.File, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT;
			}
		}
	}
}
#include <sndfile_context.hpp>

namespace xpe
{
	namespace audio
	{
		sAudioFile::~sAudioFile()
		{
			sf_close(File);
		}

		namespace context
		{
			void SetCurrentFrame(SNDFILE* file, s64 frame)
			{
				sf_seek(file, frame, SEEK_SET);
			}

			s64 ReadChunk(SNDFILE* m_File, short* data, s64 num_frames)
			{
				return sf_readf_short(m_File, data, num_frames);
			}

			bool IsAmbisonic(const sAudioFile& file)
			{
				return sf_command(file.File, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT;
			}
		}
	}
}
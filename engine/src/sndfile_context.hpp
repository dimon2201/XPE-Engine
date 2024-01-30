#pragma once

#include <sndfile.h>

namespace xpe
{
	namespace audio
	{
		struct ENGINE_API sAudioFile : public cObject
		{
			SNDFILE* File = nullptr;
			SF_INFO Info;

			sAudioFile() = default;
			~sAudioFile();
		};

		namespace context
		{
            ENGINE_API void SetCurrentFrame(SNDFILE* file, s64 frame);
            ENGINE_API s64 ReadChunk(SNDFILE* m_File, short* data, s64 num_frames);
			ENGINE_API bool IsAmbisonic(const sAudioFile& file);
		}
	}
}

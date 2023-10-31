#pragma once

#include <sndfile.h>

namespace xpe {

	namespace audio {

		using namespace core;

		struct ENGINE_API sAudioFile : public Object
		{
			SNDFILE* File = nullptr;
			SF_INFO Info;

			sAudioFile() = default;
			~sAudioFile();
		};

		namespace context {
            ENGINE_API void SetCurrentFrame(SNDFILE* file, s64 frame);
            ENGINE_API s64 ReadChunk(SNDFILE* m_File, short* data, s64 num_frames);
			ENGINE_API bool isAmbisonic(const sAudioFile& file);
		}
	}
}

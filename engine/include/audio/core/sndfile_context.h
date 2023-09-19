#pragma once
#include <../thirdparty/libsndfile/include/sndfile.h> //(todo) do something with this trash...

namespace xpe {

	namespace audio {

		using namespace xpe::core;

		struct ENGINE_API AudioFile : public Object
		{
			SNDFILE* File = nullptr;
			SF_INFO Info;

			AudioFile() = default;
			~AudioFile();
		};

		namespace context {

			void SetCurrentFrame(SNDFILE* file, s64 frame);

			s64 ReadChunk(SNDFILE* m_File, short* data, s64 num_frames);

			bool ENGINE_API isAmbisonic(const AudioFile& file);

		}
	}
}

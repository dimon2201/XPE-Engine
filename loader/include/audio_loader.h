#pragma once

#include <build.h>

#include <audio/core/sndfile_context.h>

namespace xpe {

	namespace res {

		using namespace audio;
		using namespace core;

		class LOADER_API AudioLoader final
		{
		public:
            static void Init();
            static void Free();
			static AudioFile* Load(const char* filepath);
		};
	}
}
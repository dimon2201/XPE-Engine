#pragma once

#include <build.hpp>

#include <audio/core/sndfile_context.hpp>

namespace xpe {

	namespace res {

		using namespace audio;
		using namespace core;

		class LOADER_API cAudioLoader final
		{
		public:
            static void Init();
            static void Free();
			static sAudioFile* Load(const char* filepath);
		};
	}
}
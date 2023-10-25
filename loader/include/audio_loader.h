#pragma once

#include <build.h>
#include <audio/core/sndfile_context.h>

namespace xpe {

	namespace res {

		using namespace xpe::audio;
		using namespace xpe::core;

		class LOADER_API AudioLoader : public Object
		{

		public:
			Ref<AudioFile> Load(const char* filepath);

        private:
            unordered_map<string, Ref<AudioFile>> m_Map;

		};
	}
}
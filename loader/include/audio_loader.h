#pragma once
#include <build.h>
#include <sndfile.h>

#include <audio/core/openal_context.h>
#include <audio/storages/audio_storage.h>

namespace xpe {

	namespace res {

		using namespace xpe::audio;
		using namespace xpe::core;

		class LOADER_API AudioLoader : public Object
		{
		public:

			AudioLoader(AudioStorage* audioStorage) : m_Storage(audioStorage) {}

			Ref<AudioFile> Load(const char* filepath);

		protected:

			AudioStorage* m_Storage;
		};
	}
}
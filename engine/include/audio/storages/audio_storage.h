#pragma once

#include <audio/core/sndfile_context.h>

namespace xpe {

    namespace audio {
        
        using namespace xpe::core;

        class ENGINE_API AudioStorage : public Object
        {

        public:
            Ref<AudioFile> Add(const char* filepath, const AudioFile& audio);

            void Remove(const string& filepath);

            inline Ref<AudioFile>& Get(const string& filepath) { return m_Audio[filepath]; }

            void Clear();

            inline Ref<AudioFile>& operator[](const string& ffilepath) { return Get(ffilepath); }

        protected:
            unordered_map<string, Ref<AudioFile>> m_Audio;
        };
    }
}
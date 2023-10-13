#include <audio/storages/audio_storage.h>

namespace xpe {

    namespace audio {

        Ref<AudioFile> AudioStorage::Add(const char* filepath, const AudioFile& audio)
        {
            Ref<AudioFile> audioRef;
            audioRef.Create(audio);
            m_Map.insert({filepath, audioRef });
            return audioRef;
        }

        void AudioStorage::Remove(const string& filepath)
        {
            auto it = m_Map.find(filepath);
            if (it != m_Map.end()) {
                m_Map.erase(it);
            }
        }

        void AudioStorage::Clear()
        {
            m_Map.clear();
        }

    }
}
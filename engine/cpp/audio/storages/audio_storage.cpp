#include <audio/storages/audio_storage.h>

namespace xpe {

    namespace audio {

        Ref<AudioFile> AudioStorage::Add(const char* filepath, const AudioFile& audio)
        {
            Ref<AudioFile> audioRef;
            audioRef.Create(audio);
            m_Audio.insert({ filepath, audioRef });
            return audioRef;
        }

        void AudioStorage::Remove(const string& filepath)
        {
            auto it = m_Audio.find(filepath);
            if (it != m_Audio.end()) {
                m_Audio.erase(it);
            }
        }

        void AudioStorage::Clear()
        {
            m_Audio.clear();
        }

    }
}
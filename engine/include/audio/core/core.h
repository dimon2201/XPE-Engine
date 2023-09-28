#pragma once

namespace xpe {

    namespace audio {

        enum eAudioState {
            INITIAL,
            PLAYING,
            PAUSED,
            STOPPED
        };

        enum eAudioError {
            NONE,
            INVALID_NAME,
            INVALID_ENUM,
            INVALID_VALUE,
            INVALID_OPERATION,
            OUT_OF_MEMORY
        };

    }

}
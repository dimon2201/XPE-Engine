#pragma once

namespace xpe {

    namespace audio {

        using namespace core;

        enum eAudioState {
            INITIAL,
            PLAYING,
            PAUSED,
            STOPPED
        };
        JSON_ENUM(eAudioState, {
            { eAudioState::INITIAL, "INITIAL" },
            { eAudioState::PLAYING, "PLAYING" },
            { eAudioState::PAUSED,  "PAUSED" },
            { eAudioState::STOPPED, "STOPPED" },
        })

        enum eAudioError {
            NONE,
            INVALID_NAME,
            INVALID_ENUM,
            INVALID_VALUE,
            INVALID_OPERATION,
            OUT_OF_MEMORY
        };
        JSON_ENUM(eAudioError, {
            { eAudioError::NONE,              "INITIAL" },
            { eAudioError::INVALID_NAME,      "INVALID_NAME" },
            { eAudioError::INVALID_ENUM,      "INVALID_ENUM" },
            { eAudioError::INVALID_VALUE,     "INVALID_VALUE" },
            { eAudioError::INVALID_OPERATION, "INVALID_OPERATION" },
            { eAudioError::OUT_OF_MEMORY,     "OUT_OF_MEMORY" },
        })

        struct ENGINE_API sAudioSource
        {
            u32 Id = 0;

            eAudioState State = eAudioState::INITIAL;

            f32 Gain = 1;                   // affects audio volume
            f32 Pitch = 1;                  // affects playback speed

            f32 RefDistance = 1.0f;         // Distance at which the sound is heard clearly
            f32 MaxDistance = 100.0f;       // Maximum distance at which the sound is heard
            f32 RollOffFactor = 1.0f;       // Volume reduction factor with distance
            f32 ConeInnerAngle = 180.0f;    // Angle of the inner zone of the cone
            f32 ConeOuterAngle = 180.0f;    // Angle of the outer zone of the cone

            bool Looping = false;
        };

        JSON(
            sAudioSource,
            Id,
            State,
            Gain,
            Pitch,
            RefDistance,
            MaxDistance
        )
    }

}
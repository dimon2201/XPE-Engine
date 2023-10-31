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

        enum eAudioError {
            NONE,
            INVALID_NAME,
            INVALID_ENUM,
            INVALID_VALUE,
            INVALID_OPERATION,
            OUT_OF_MEMORY
        };

        struct ENGINE_API sAudioSource
        {
            u32 Id = 0;

            eAudioState State = eAudioState::INITIAL;

            glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
            glm::vec3 Velocity = { 0.0f, 0.0f, 0.0f };

            f32 Gain = 1;                   // affects audio volume
            f32 Pitch = 1;                  // affects playback speed

            f32 RefDistance = 1.0f;         // Distance at which the sound is heard clearly
            f32 MaxDistance = 100.0f;       // Maximum distance at which the sound is heard
            f32 RollOffFactor = 1.0f;       // Volume reduction factor with distance
            f32 ConeInnerAngle = 180.0f;    // Angle of the inner zone of the cone
            f32 ConeOuterAngle = 180.0f;    // Angle of the outer zone of the cone

            bool Looping = false;
        };

        Json(
                sAudioSource,
                Id,
                State,
                Position,
                Velocity,
                Gain,
                Pitch,
                RefDistance,
                MaxDistance
        )

    }

}
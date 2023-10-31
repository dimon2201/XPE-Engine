#pragma once

namespace xpe {

    namespace anim {

        using namespace core;

        struct ENGINE_API sKeyPosition final
        {
            float Timestamp;
            glm::vec3 Position;

            sKeyPosition() = default;
            sKeyPosition(float timestamp, const glm::vec3& position) : Timestamp(timestamp), Position(position) {}
        };

        Json(sKeyPosition, Timestamp, Position)

        struct ENGINE_API sKeyRotate final
        {
            float Timestamp;
            glm::quat Orientation;

            sKeyRotate() = default;
            sKeyRotate(float timestamp, const glm::quat& orientation) : Timestamp(timestamp), Orientation(orientation) {}
        };

        Json(sKeyRotate, Timestamp, Orientation)

        struct ENGINE_API sKeyScale final
        {
            float Timestamp;
            glm::vec3 Scale;

            sKeyScale() = default;
            sKeyScale(float timestamp, const glm::vec3& scale) : Timestamp(timestamp), Scale(scale) {}
        };

        Json(sKeyScale, Timestamp, Scale)

        struct ENGINE_API sAnimationNode final
        {
            string Name;
            glm::mat4 Transform;
            vector<sAnimationNode> Children;
        };

        Json(sAnimationNode, Name, Transform, Children)

        struct ENGINE_API sAnimation
        {
            bool Play;
            float Duration;
            int TicksPerSecond;
            sAnimationNode Root;
        };

        Json(sAnimation, Duration, TicksPerSecond, Root)

    }

}
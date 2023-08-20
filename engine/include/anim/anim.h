#pragma once

namespace xpe {

    namespace anim {

        using namespace core;

        struct ENGINE_API KeyPosition final
        {
            float Timestamp;
            glm::vec3 Position;

            KeyPosition() = default;
            KeyPosition(float timestamp, const glm::vec3& position) : Timestamp(timestamp), Position(position) {}
        };

        Json(KeyPosition, Timestamp, Position)

        struct ENGINE_API KeyRotate final
        {
            float Timestamp;
            glm::quat Orientation;

            KeyRotate() = default;
            KeyRotate(float timestamp, const glm::quat& orientation) : Timestamp(timestamp), Orientation(orientation) {}
        };

        Json(KeyRotate, Timestamp, Orientation)

        struct ENGINE_API KeyScale final
        {
            float Timestamp;
            glm::vec3 Scale;

            KeyScale() = default;
            KeyScale(float timestamp, const glm::vec3& scale) : Timestamp(timestamp), Scale(scale) {}
        };

        Json(KeyScale, Timestamp, Scale)

        struct ENGINE_API AnimationNode final
        {
            string Name;
            glm::mat4 Transform;
            vector<AnimationNode> Children;
        };

        Json(AnimationNode, Name, Transform, Children)

        struct ENGINE_API Animation final
        {
            float Duration;
            int TicksPerSecond;
            AnimationNode Root;
        };

        Json(Animation, Duration, TicksPerSecond, Root)

    }

}
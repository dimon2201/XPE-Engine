#pragma once

namespace xpe {

    namespace math {

        struct ENGINE_API sModelMatrix
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Rotation = { 0, 0, 0 };
            glm::vec3 Scale = { 1, 1, 1 };
        };

        JSON(
            sModelMatrix,
            Position,
            Rotation,
            Scale
        )

        struct ENGINE_API sViewMatrix
        {
            glm::vec3 Position = { 0, 0, 1 };
            glm::vec3 Front = { 0, 0, 0 };
            glm::vec3 Up = { 0, 1, 0 };
        };

        JSON(
            sViewMatrix,
            Position,
            Front,
            Up
        )

        struct ENGINE_API sOrthoMatrix
        {
            float Left = 0;
            float Right = 1;
            float Bottom = 0;
            float Top = 1;
            float Near = 0.1f;
            float Far = 1.0f;
        };

        JSON(
            sOrthoMatrix,
            Left,
            Right,
            Bottom,
            Top,
            Near,
            Far
        )

        struct ENGINE_API sPerspectiveMatrix
        {
            float FovDegree = 60.0f;
            float AspectRatio = 1.3f;
            float Near = 0.1f;
            float Far = 100.0f;
        };

        JSON(
            sPerspectiveMatrix,
            FovDegree,
            AspectRatio,
            Near,
            Far
        )

    }

}
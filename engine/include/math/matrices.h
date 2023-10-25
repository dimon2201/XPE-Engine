#pragma once

namespace xpe {

    namespace math {

        struct ENGINE_API ModelMatrix final
        {
            glm::vec3 Translation = { 0, 0, 0 };
            glm::vec3 Rotation = { 0, 0, 0 };
            glm::vec3 Scale = { 1, 1, 1 };
        };

        Json(
            ModelMatrix,
            Translation,
            Rotation,
            Scale
        )

        glm::mat4 ENGINE_API ModelMatrixUpdate(const ModelMatrix& modelMatrix);

        struct ENGINE_API ViewMatrix final
        {
            glm::vec3 Position = { 0, 0, 1 };
            glm::vec3 Front = {0, 0, 0 };
            glm::vec3 Up = { 0, 1, 0 };
        };

        Json(
            ViewMatrix,
            Position,
            Front,
            Up
        )

        glm::mat4 ENGINE_API ViewMatrixUpdate(const ViewMatrix& viewMatrix);

        struct ENGINE_API OrthoMatrix final
        {
            float Left = 0;
            float Right = 1;
            float Bottom = 0;
            float Top = 1;
            float Near = 0.1f;
            float Far = 1.0f;
        };

        Json(
            OrthoMatrix,
            Left,
            Right,
            Bottom,
            Top,
            Near,
            Far
        )

        glm::mat4 ENGINE_API OrthoMatrixUpdate(const OrthoMatrix& orthoMatrix);

        struct ENGINE_API PerspectiveMatrix final
        {
            float FovDegree = 45.0f;
            float AspectRatio = 1.0f;
            float Near = 0.1f;
            float Far = 100.0f;
        };

        Json(
            PerspectiveMatrix,
            FovDegree,
            AspectRatio,
            Near,
            Far
        )

        glm::mat4 ENGINE_API PerspectiveMatrixUpdate(const PerspectiveMatrix& perspectiveMatrix);

    }

}
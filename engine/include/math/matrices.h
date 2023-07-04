#pragma once

namespace xpe {

    namespace math {

        struct ENGINE_API ModelMatrix final {
            glm::vec3 Translation = { 0, 0, 0 };
            glm::vec3 Rotation = { 0, 0, 0 };
            glm::vec3 Scale = { 1, 1, 1 };
        };

        glm::mat4 ENGINE_API ModelMatrixUpdate(const ModelMatrix& modelMatrix);

        struct ENGINE_API Model2dMatrix final {
            glm::vec2 Translation = { 0, 0 };
            float Rotation = 0;
            glm::vec2 Scale = { 1, 1 };
        };

        glm::mat4 ENGINE_API Model2dMatrixUpdate(const Model2dMatrix& model2DMatrix);

        struct ENGINE_API ViewMatrix final {
            glm::vec3 Eye = { 0, 0, 0 };
            glm::vec3 Center = { 0, 0, 0 };
            glm::vec3 Up = { 0, 1, 0 };
        };

        glm::mat4 ENGINE_API ViewMatrixUpdate(const ViewMatrix& viewMatrix);

        struct ENGINE_API OrthoMatrix final {
            float Left = 0;
            float Right = 0;
            float Bottom = 0;
            float Top = 0;
            float Near = 0.1f;
            float Far = 100.0f;
        };

        glm::mat4 ENGINE_API OrthoMatrixUpdate(const OrthoMatrix& orthoMatrix);

        struct ENGINE_API PerspectiveMatrix final {
            float FovDegree = 45.0f;
            float AspectRatio = 1.0f;
            float Near = 0.1f;
            float Far = 100.0f;
        };

        glm::mat4 ENGINE_API PerspectiveMatrixUpdate(const PerspectiveMatrix& perspectiveMatrix);

    }

}
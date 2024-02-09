#pragma once

namespace xpe
{
    namespace math
    {
        template<typename T>
        void Clamp(T& value, const T min, const T max)
        {
            if (value < min)
            {
                value = min;
            }
            else if (value > max)
            {
                value = max;
            }
        }

        template<typename T>
        T Min(const T& right, const T& left)
        {
            return right < left ? right : left;
        }

        template<typename T>
        T Max(const T& right, const T& left)
        {
            return right > left ? right : left;
        }

        template<typename T>
        T Random(const T& min, const T& max)
        {
            static std::default_random_engine generator;
            std::uniform_real_distribution<T> distribution(min, max);
            return distribution(generator);
        }

        struct ENGINE_API sRay final
        {
            glm::vec4 Origin = { 0, 0, 0, 1 };
            glm::vec4 Direction = { 0, 0, 0, 1 };
        };

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

        class ENGINE_API MMath final
        {

        public:
            // world space -> local space
            static sRay ToLocalSpace(const sRay& ray, const glm::mat4& modelMatrix);
            // local space -> world space
            static sRay ToWorldSpace(const sRay& ray, const glm::mat4& modelMatrix);
            // view space -> world space
            static sRay ToWorldSpaceInverse(const sRay& ray, const glm::mat4& viewMatrix);
            // world space -> view space
            static sRay ToViewSpace(const sRay& ray, const glm::mat4& viewMatrix);
            // clip space -> view space
            static sRay ToViewSpaceInverse(const sRay& ray, const float fov, const float aspectRatio);
            static sRay ToViewSpaceInverse(const sRay& ray, const glm::mat4& perspectiveMatrix);
            // view space -> clip space
            static sRay ToClipSpace(const sRay& ray, const float fov, const float aspectRatio);
            static sRay ToClipSpace(const sRay& ray, const glm::mat4& perspectiveMatrix);
            // NDC space -> clip space
            static sRay ToClipSpaceInverse(const sRay& ray);
            // clip space -> NDC space
            static sRay ToNDCSpace(const sRay& ray);
            // screen space -> NDC space
            static sRay ToNDCSpaceInverse(const sRay& ray, int width, int height);
            // NDC space -> screen space
            static sRay ToScreenSpace(const sRay& ray, int width, int height);

            static glm::mat4 UpdateModelMatrix(const sModelMatrix& modelMatrix);
            static glm::mat4 UpdateNormalMatrix(const glm::mat4& modelMatrix);
            static glm::mat4 UpdateViewMatrix(const sViewMatrix& viewMatrix);
            static glm::mat4 UpdateOrthoMatrix(const sOrthoMatrix& orthoMatrix);
            static glm::mat4 UpdatePerspectiveMatrix(const sPerspectiveMatrix& perspectiveMatrix);
            static glm::mat4 UpdateLightMatrix(const sOrthoMatrix& orthoMatrix, const sViewMatrix& viewMatrix);
            static glm::mat4 UpdateLightMatrix(const sPerspectiveMatrix& perspectiveMatrix, const sViewMatrix& viewMatrix);
        };
    }
}

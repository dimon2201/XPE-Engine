#pragma once

namespace xpe {

    namespace math {

        struct ENGINE_API Ray final {
            glm::vec4 Origin = { 0, 0, 0, 1 };
            glm::vec4 Direction = { 0, 0, 0, 1 };
        };

        class ENGINE_API RayCast final {

        public:
            // world space -> local space
            static Ray ToLocalSpace(const Ray& ray, const glm::mat4& modelMatrix);
            // local space -> world space
            static Ray ToWorldSpace(const Ray& ray, const glm::mat4& modelMatrix);
            // view space -> world space
            static Ray ToWorldSpaceInverse(const Ray& ray, const glm::mat4& viewMatrix);
            // world space -> view space
            static Ray ToViewSpace(const Ray& ray, const glm::mat4& viewMatrix);
            // clip space -> view space
            static Ray ToViewSpaceInverse(const Ray& ray, const float fov, const float aspectRatio);
            static Ray ToViewSpaceInverse(const Ray& ray, const glm::mat4& perspectiveMatrix);
            // view space -> clip space
            static Ray ToClipSpace(const Ray& ray, const float fov, const float aspectRatio);
            static Ray ToClipSpace(const Ray& ray, const glm::mat4& perspectiveMatrix);
            // NDC space -> clip space
            static Ray ToClipSpaceInverse(const Ray& ray);
            // clip space -> NDC space
            static Ray ToNDCSpace(const Ray& ray);
            // screen space -> NDC space
            static Ray ToNDCSpaceInverse(const Ray& ray, int width, int height);
            // NDC space -> screen space
            static Ray ToScreenSpace(const Ray& ray, int width, int height);
        };

    }

}
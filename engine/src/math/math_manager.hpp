#pragma once

#include <math/math.hpp>
#include <math/ray.hpp>
#include <math/matrices.hpp>
#include <math/transform.hpp>

namespace xpe {

    namespace math {

        class ENGINE_API cMathManager final {
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

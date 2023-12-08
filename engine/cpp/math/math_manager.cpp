
#include "math/math_manager.h"

namespace xpe {

    namespace math {

        sRay cMathManager::ToLocalSpace(const sRay &ray, const glm::mat4 &modelMatrix)
        {
            return { glm::inverse(modelMatrix) * ray.Origin };
        }

        sRay cMathManager::ToWorldSpace(const sRay &ray, const glm::mat4 &modelMatrix)
        {
            return { modelMatrix * ray.Origin };
        }

        sRay cMathManager::ToWorldSpaceInverse(const sRay &ray, const glm::mat4 &viewMatrix)
        {
            return { glm::inverse(viewMatrix) * ray.Origin };
        }

        sRay cMathManager::ToViewSpace(const sRay &ray, const glm::mat4 &viewMatrix)
        {
            return { viewMatrix * ray.Origin };
        }

        sRay cMathManager::ToViewSpaceInverse(const sRay &ray, const float fov, const float aspectRatio)
        {
            float focalLength = 1.0f / glm::tan(glm::radians(fov / 2.0f));
            glm::vec4 newOrigin = { ray.Origin.x / focalLength, ray.Origin.y * aspectRatio / focalLength, 1.0f, ray.Origin.w };
            return { newOrigin };
        }

        sRay cMathManager::ToViewSpaceInverse(const sRay &ray, const glm::mat4 &perspectiveMatrix)
        {
            return { glm::inverse(perspectiveMatrix) * ray.Origin };
        }

        sRay cMathManager::ToClipSpace(const sRay &ray, const float fov, const float aspectRatio)
        {
            float focalLength = 1.0f / glm::tan(glm::radians(fov / 2.0f));
            glm::vec4 newOrigin = { ray.Origin.x * focalLength, ray.Origin.y * focalLength / aspectRatio, 1.0f, ray.Origin.w };
            return { newOrigin };
        }

        sRay cMathManager::ToClipSpace(const sRay &ray, const glm::mat4 &perspectiveMatrix)
        {
            return {perspectiveMatrix * ray.Origin };
        }

        sRay cMathManager::ToClipSpaceInverse(const sRay &ray)
        {
            glm::vec4 newOrigin = { ray.Origin.x * ray.Origin.w, ray.Origin.y * ray.Origin.w, ray.Origin.z * ray.Origin.w, ray.Origin.w };
            return { newOrigin };
        }

        sRay cMathManager::ToNDCSpace(const sRay &ray)
        {
            glm::vec4 newOrigin = { ray.Origin.x / ray.Origin.w, ray.Origin.y / ray.Origin.w, ray.Origin.z / ray.Origin.w, ray.Origin.w / ray.Origin.w };
            return { newOrigin };
        }

        sRay cMathManager::ToNDCSpaceInverse(const sRay &ray, int width, int height)
        {
            glm::vec4 newOrigin = { (ray.Origin.x * 2 / (float) width) - 1, 1 - (ray.Origin.y * 2 / (float) height), 1.0f, ray.Origin.w };
            return { newOrigin };
        }

        sRay cMathManager::ToScreenSpace(const sRay &ray, int width, int height)
        {
            glm::vec4 newOrigin = { (ray.Origin.x + 1) * (float) width / 2, (ray.Origin.y + 1) * (float) height / 2, 1.0f, ray.Origin.w };
            return { newOrigin };
        }

        glm::mat4 cMathManager::UpdateModelMatrix(const sModelMatrix &modelMatrix)
        {
            glm::mat4 m = glm::mat4(1.0f);
            m = glm::translate(m, modelMatrix.Position);
            m = m * glm::toMat4(glm::quat(modelMatrix.Rotation));
            m = glm::scale(m, modelMatrix.Scale);
            return m;
        }

        glm::mat4 cMathManager::UpdateNormalMatrix(const glm::mat4 &modelMatrix)
        {
            return glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
        }

        glm::mat4 cMathManager::UpdateViewMatrix(const sViewMatrix &viewMatrix)
        {
            return glm::lookAt(viewMatrix.Position, viewMatrix.Front, viewMatrix.Up);
        }

        glm::mat4 cMathManager::UpdateOrthoMatrix(const sOrthoMatrix &orthoMatrix)
        {
            return glm::ortho(
                    orthoMatrix.Left,
                    orthoMatrix.Right,
                    orthoMatrix.Bottom,
                    orthoMatrix.Top,
                    orthoMatrix.Near,
                    orthoMatrix.Far
            );
        }

        glm::mat4 cMathManager::UpdatePerspectiveMatrix(const sPerspectiveMatrix &perspectiveMatrix)
        {
            return glm::perspective(
                    glm::radians(perspectiveMatrix.FovDegree),
                    perspectiveMatrix.AspectRatio,
                    perspectiveMatrix.Near,
                    perspectiveMatrix.Far
            );
        }

        glm::mat4 cMathManager::UpdateLightMatrix(const sOrthoMatrix& orthoMatrix, const sViewMatrix& viewMatrix)
        {
            return UpdateOrthoMatrix(orthoMatrix) * UpdateViewMatrix(viewMatrix);
        }

        glm::mat4 cMathManager::UpdateLightMatrix(const sPerspectiveMatrix& perspectiveMatrix, const sViewMatrix& viewMatrix)
        {
            return UpdatePerspectiveMatrix(perspectiveMatrix) * UpdateViewMatrix(viewMatrix);
        }

    }

}
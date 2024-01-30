#include <math_manager.hpp>

namespace xpe
{
    namespace math
    {
        sRay MMath::ToLocalSpace(const sRay &ray, const glm::mat4 &modelMatrix)
        {
            return { glm::inverse(modelMatrix) * ray.Origin };
        }

        sRay MMath::ToWorldSpace(const sRay &ray, const glm::mat4 &modelMatrix)
        {
            return { modelMatrix * ray.Origin };
        }

        sRay MMath::ToWorldSpaceInverse(const sRay &ray, const glm::mat4 &viewMatrix)
        {
            return { glm::inverse(viewMatrix) * ray.Origin };
        }

        sRay MMath::ToViewSpace(const sRay &ray, const glm::mat4 &viewMatrix)
        {
            return { viewMatrix * ray.Origin };
        }

        sRay MMath::ToViewSpaceInverse(const sRay &ray, const float fov, const float aspectRatio)
        {
            float focalLength = 1.0f / glm::tan(glm::radians(fov / 2.0f));
            glm::vec4 newOrigin = { ray.Origin.x / focalLength, ray.Origin.y * aspectRatio / focalLength, 1.0f, ray.Origin.w };
            return { newOrigin };
        }

        sRay MMath::ToViewSpaceInverse(const sRay &ray, const glm::mat4 &perspectiveMatrix)
        {
            return { glm::inverse(perspectiveMatrix) * ray.Origin };
        }

        sRay MMath::ToClipSpace(const sRay &ray, const float fov, const float aspectRatio)
        {
            float focalLength = 1.0f / glm::tan(glm::radians(fov / 2.0f));
            glm::vec4 newOrigin = { ray.Origin.x * focalLength, ray.Origin.y * focalLength / aspectRatio, 1.0f, ray.Origin.w };
            return { newOrigin };
        }

        sRay MMath::ToClipSpace(const sRay &ray, const glm::mat4 &perspectiveMatrix)
        {
            return {perspectiveMatrix * ray.Origin };
        }

        sRay MMath::ToClipSpaceInverse(const sRay &ray)
        {
            glm::vec4 newOrigin = { ray.Origin.x * ray.Origin.w, ray.Origin.y * ray.Origin.w, ray.Origin.z * ray.Origin.w, ray.Origin.w };
            return { newOrigin };
        }

        sRay MMath::ToNDCSpace(const sRay &ray)
        {
            glm::vec4 newOrigin = { ray.Origin.x / ray.Origin.w, ray.Origin.y / ray.Origin.w, ray.Origin.z / ray.Origin.w, ray.Origin.w / ray.Origin.w };
            return { newOrigin };
        }

        sRay MMath::ToNDCSpaceInverse(const sRay &ray, int width, int height)
        {
            glm::vec4 newOrigin = { (ray.Origin.x * 2 / (float) width) - 1, 1 - (ray.Origin.y * 2 / (float) height), 1.0f, ray.Origin.w };
            return { newOrigin };
        }

        sRay MMath::ToScreenSpace(const sRay &ray, int width, int height)
        {
            glm::vec4 newOrigin = { (ray.Origin.x + 1) * (float) width / 2, (ray.Origin.y + 1) * (float) height / 2, 1.0f, ray.Origin.w };
            return { newOrigin };
        }

        glm::mat4 MMath::UpdateModelMatrix(const sModelMatrix &modelMatrix)
        {
            glm::mat4 m = glm::mat4(1.0f);
            m = glm::translate(m, modelMatrix.Position);
            m = m * glm::toMat4(glm::quat(modelMatrix.Rotation));
            m = glm::scale(m, modelMatrix.Scale);
            return m;
        }

        glm::mat4 MMath::UpdateNormalMatrix(const glm::mat4 &modelMatrix)
        {
            return glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
        }

        glm::mat4 MMath::UpdateViewMatrix(const sViewMatrix &viewMatrix)
        {
            return glm::lookAt(viewMatrix.Position, viewMatrix.Front, viewMatrix.Up);
        }

        glm::mat4 MMath::UpdateOrthoMatrix(const sOrthoMatrix &orthoMatrix)
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

        glm::mat4 MMath::UpdatePerspectiveMatrix(const sPerspectiveMatrix &perspectiveMatrix)
        {
            return glm::perspective(
                    glm::radians(perspectiveMatrix.FovDegree),
                    perspectiveMatrix.AspectRatio,
                    perspectiveMatrix.Near,
                    perspectiveMatrix.Far
            );
        }

        glm::mat4 MMath::UpdateLightMatrix(const sOrthoMatrix& orthoMatrix, const sViewMatrix& viewMatrix)
        {
            return UpdateOrthoMatrix(orthoMatrix) * UpdateViewMatrix(viewMatrix);
        }

        glm::mat4 MMath::UpdateLightMatrix(const sPerspectiveMatrix& perspectiveMatrix, const sViewMatrix& viewMatrix)
        {
            return UpdatePerspectiveMatrix(perspectiveMatrix) * UpdateViewMatrix(viewMatrix);
        }
    }
}
namespace xpe {

    namespace math {

        sRay MathManager::ToLocalSpace(const sRay &ray, const glm::mat4 &modelMatrix) {
            return { glm::inverse(modelMatrix) * ray.Origin };
        }

        sRay MathManager::ToWorldSpace(const sRay &ray, const glm::mat4 &modelMatrix) {
            return { modelMatrix * ray.Origin };
        }

        sRay MathManager::ToWorldSpaceInverse(const sRay &ray, const glm::mat4 &viewMatrix) {
            return { glm::inverse(viewMatrix) * ray.Origin };
        }

        sRay MathManager::ToViewSpace(const sRay &ray, const glm::mat4 &viewMatrix) {
            return { viewMatrix * ray.Origin };
        }

        sRay MathManager::ToViewSpaceInverse(const sRay &ray, const float fov, const float aspectRatio) {
            float focalLength = 1.0f / glm::tan(glm::radians(fov / 2.0f));
            glm::vec4 newOrigin = { ray.Origin.x / focalLength, ray.Origin.y * aspectRatio / focalLength, 1.0f, ray.Origin.w };
            return { newOrigin };
        }

        sRay MathManager::ToViewSpaceInverse(const sRay &ray, const glm::mat4 &perspectiveMatrix) {
            return { glm::inverse(perspectiveMatrix) * ray.Origin };
        }

        sRay MathManager::ToClipSpace(const sRay &ray, const float fov, const float aspectRatio) {
            float focalLength = 1.0f / glm::tan(glm::radians(fov / 2.0f));
            glm::vec4 newOrigin = { ray.Origin.x * focalLength, ray.Origin.y * focalLength / aspectRatio, 1.0f, ray.Origin.w };
            return { newOrigin };
        }

        sRay MathManager::ToClipSpace(const sRay &ray, const glm::mat4 &perspectiveMatrix) {
            return {perspectiveMatrix * ray.Origin };
        }

        sRay MathManager::ToClipSpaceInverse(const sRay &ray) {
            glm::vec4 newOrigin = { ray.Origin.x * ray.Origin.w, ray.Origin.y * ray.Origin.w, ray.Origin.z * ray.Origin.w, ray.Origin.w };
            return { newOrigin };
        }

        sRay MathManager::ToNDCSpace(const sRay &ray) {
            glm::vec4 newOrigin = { ray.Origin.x / ray.Origin.w, ray.Origin.y / ray.Origin.w, ray.Origin.z / ray.Origin.w, ray.Origin.w / ray.Origin.w };
            return { newOrigin };
        }

        sRay MathManager::ToNDCSpaceInverse(const sRay &ray, int width, int height) {
            glm::vec4 newOrigin = { (ray.Origin.x * 2 / (float) width) - 1, 1 - (ray.Origin.y * 2 / (float) height), 1.0f, ray.Origin.w };
            return { newOrigin };
        }

        sRay MathManager::ToScreenSpace(const sRay &ray, int width, int height) {
            glm::vec4 newOrigin = { (ray.Origin.x + 1) * (float) width / 2, (ray.Origin.y + 1) * (float) height / 2, 1.0f, ray.Origin.w };
            return { newOrigin };
        }

        glm::mat4 MathManager::UpdateModelMatrix(const sModelMatrix &modelMatrix) {
            glm::mat4 m = glm::mat4(1.0f);
            m = glm::translate(m, modelMatrix.Position);
            m = m * glm::toMat4(glm::quat(modelMatrix.Rotation));
            m = glm::scale(m, modelMatrix.Scale);
            return m;
        }

        glm::mat4 MathManager::UpdateNormalMatrix(const glm::mat4 &modelMatrix)
        {
            return glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
        }

        glm::mat4 MathManager::UpdateViewMatrix(const sViewMatrix &viewMatrix) {
            return math::lookAt(viewMatrix.Position, viewMatrix.Front, viewMatrix.Up);
        }

        glm::mat4 MathManager::UpdateOrthoMatrix(const sOrthoMatrix &orthoMatrix) {
            return math::ortho(
                    orthoMatrix.Left,
                    orthoMatrix.Right,
                    orthoMatrix.Bottom,
                    orthoMatrix.Top,
                    orthoMatrix.Near,
                    orthoMatrix.Far
            );
        }

        glm::mat4 MathManager::UpdatePerspectiveMatrix(const sPerspectiveMatrix &perspectiveMatrix) {
            return math::perspective(
                    glm::radians(perspectiveMatrix.FovDegree),
                    perspectiveMatrix.AspectRatio,
                    perspectiveMatrix.Near,
                    perspectiveMatrix.Far
            );
        }

        glm::mat4 MathManager::UpdateLightMatrix(const sOrthoMatrix& orthoMatrix, const sViewMatrix& viewMatrix) {
            return UpdateOrthoMatrix(orthoMatrix) * UpdateViewMatrix(viewMatrix);
        }

    }

}
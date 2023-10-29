namespace xpe {

    namespace math {

        glm::mat4 ModelMatrixUpdate(const ModelMatrix &modelMatrix) {
            glm::mat4 m = glm::mat4(1.0f);
            m = glm::translate(m, modelMatrix.Translation);
            //m = glm::rotate(m, glm::radians(modelMatrix.Rotation.x), glm::vec3(1, 0, 0));
            //m = glm::rotate(m, glm::radians(modelMatrix.Rotation.y), glm::vec3(0, 1, 0));
            //m = glm::rotate(m, glm::radians(modelMatrix.Rotation.z), glm::vec3(0, 0, 1));
            m = m * glm::toMat4(glm::quat(modelMatrix.Rotation));
            m = glm::scale(m, modelMatrix.Scale);
            return m;
        }

        glm::mat4 ViewMatrixUpdate(const ViewMatrix &viewMatrix) {
            return math::lookAt(viewMatrix.Position, viewMatrix.Front, viewMatrix.Up);
        }

        glm::mat4 OrthoMatrixUpdate(const OrthoMatrix &orthoMatrix) {
            return math::ortho(
                    orthoMatrix.Left,
                    orthoMatrix.Right,
                    orthoMatrix.Bottom,
                    orthoMatrix.Top,
                    orthoMatrix.Near,
                    orthoMatrix.Far
            );
        }

        glm::mat4 PerspectiveMatrixUpdate(const PerspectiveMatrix &perspectiveMatrix) {
            return math::perspective(
                    glm::radians(perspectiveMatrix.FovDegree),
                    perspectiveMatrix.AspectRatio,
                    perspectiveMatrix.Near,
                    perspectiveMatrix.Far
            );
        }

        glm::mat4 LightMatrixUpdate(const OrthoMatrix& orthoMatrix, const ViewMatrix& viewMatrix) {
            return OrthoMatrixUpdate(orthoMatrix) * ViewMatrixUpdate(viewMatrix);
        }

    }

}
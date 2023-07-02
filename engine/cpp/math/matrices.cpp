namespace xpe {

    namespace math {

        glm::mat4 ModelMatrixUpdate(const ModelMatrix &modelMatrix) {
            return {};
        }

        glm::mat4 Model2dMatrixUpdate(const Model2dMatrix &model2DMatrix) {
            return {};
        }

        glm::mat4 ViewMatrixUpdate(const ViewMatrix &viewMatrix) {
            return math::lookAt(viewMatrix.Eye, viewMatrix.Center, viewMatrix.Up);
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
    }

}
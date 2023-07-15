#include <controls/camera.h>

namespace xpe {

    namespace control {

        Camera::Camera(CameraBuffer* cameraBuffer)
        : m_CameraBuffer(cameraBuffer) {}

        PerspectiveCamera::PerspectiveCamera(
                CameraBuffer *cameraBuffer,
                PerspectiveCameraComponent *component
        ) : Camera(cameraBuffer),
            Component(component),
            MaxFovDegree(component->Projection.FovDegree)
        {
            m_CameraBuffer->SetCamera(component);
            m_CameraBuffer->Flush();
            Input::WindowFrameResizedEvents.AddEvent(this, core::OnWindowFrameResized<PerspectiveCamera>, 2);
            Input::ScrollChangedEvents.AddEvent(this, core::OnScrollChanged<PerspectiveCamera>, 2);
            Input::CursorMovedEvents.AddEvent(this, core::OnCursorMoved<PerspectiveCamera>, 2);
        }

        void PerspectiveCamera::Move() {
            float distance = MoveSpeed;
            auto& position = Component->Position;
            auto front = glm::normalize(Component->Front);
            auto up = glm::normalize(Component->Up);
            auto right = glm::cross(front, up);

            if (Input::KeyPressed(KeyMoveForward)) {
                position += front * distance;
            }

            else if (Input::KeyPressed(KeyMoveLeft)) {
                position += right * distance;
            }

            else if (Input::KeyPressed(KeyMoveBackward)) {
                position -= front * distance;
            }

            else if (Input::KeyPressed(KeyMoveRight)) {
                position -= right * distance;
            }

            else
                return;

            m_CameraBuffer->SetView(Component);
            m_CameraBuffer->SetPosition(Component);
            m_CameraBuffer->Flush();
        }

        void PerspectiveCamera::ZoomIn() {
            auto& fov = Component->Projection.FovDegree;

            fov -= (float) ZoomSpeed;
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            m_CameraBuffer->SetPerspectiveProjection(Component->Index, Component->Projection);
            m_CameraBuffer->Flush();
        }

        void PerspectiveCamera::ZoomOut() {
            auto& fov = Component->Projection.FovDegree;

            fov += (float) ZoomSpeed;
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            m_CameraBuffer->SetPerspectiveProjection(Component->Index, Component->Projection);
            m_CameraBuffer->Flush();
        }

        void PerspectiveCamera::ScrollChanged(const double x, const double y) {
            auto& fov = Component->Projection.FovDegree;

            fov -= (float) y * ZoomSpeed;
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            m_CameraBuffer->SetPerspectiveProjection(Component->Index, Component->Projection);
            m_CameraBuffer->Flush();
        }

        void PerspectiveCamera::Look(const double x, const double y) {
            Input::CaptureCursor(x, y);

            auto& cursorDelta = Input::GetMouseCursor().Delta;
            auto& front = Component->Front;
            auto& up = Component->Up;

            if (cursorDelta.x != 0 || cursorDelta.y != 0) {
                float pitchDt = cursorDelta.y * VerticalSensitivity;
                float yawDt = cursorDelta.x * HorizontalSensitivity;
                glm::vec3 right = glm::cross(front, up);
                glm::quat quatRight = glm::normalize(glm::angleAxis(-pitchDt, right));
                glm::quat quatUp = glm::normalize(glm::angleAxis(yawDt, up));
                glm::quat quatFront = glm::cross(quatRight, quatUp);
                up = glm::rotate(quatUp, up);
                front = glm::rotate(quatFront, front);
                m_CameraBuffer->SetView(Component);
                m_CameraBuffer->Flush();
//                LogGLM("Look Up vector", up);
            }
        }

        void PerspectiveCamera::WindowFrameResized(int width, int height) {
            Component->Projection.AspectRatio = static_cast<float>(width) / static_cast<float>(height);
            m_CameraBuffer->SetPerspectiveProjection(Component->Index, Component->Projection);
            m_CameraBuffer->Flush();
        }

        void PerspectiveCamera::CursorMoved(const double x, const double y) {
            Look(x, y);
        }

        OrthoCamera::OrthoCamera(
            CameraBuffer *cameraBuffer,
            OrthoCameraComponent *component
        ) : Camera(cameraBuffer), Component(component) {
            m_CameraBuffer->SetCamera(component);
            m_CameraBuffer->Flush();
            Input::WindowFrameResizedEvents.AddEvent(this, core::OnWindowFrameResized<OrthoCamera>, 2);
            Input::ScrollChangedEvents.AddEvent(this, core::OnScrollChanged<OrthoCamera>, 2);
            Input::CursorMovedEvents.AddEvent(this, core::OnCursorMoved<OrthoCamera>, 2);
        }

        void OrthoCamera::Move() {
            // todo ortho camera move function
        }

        void OrthoCamera::ZoomIn() {
            // todo ortho camera zoom in function
        }

        void OrthoCamera::ZoomOut() {
            // todo ortho camera zoom out function
        }

        void OrthoCamera::Look(const double x, const double y) {
            // todo ortho camera look function
        }

        void OrthoCamera::WindowFrameResized(int w, int h) {
            Component->Projection.Left = w;
            Component->Projection.Top = h;
            m_CameraBuffer->SetOrthoProjection(Component->Index, Component->Projection);
            m_CameraBuffer->Flush();
        }

        void OrthoCamera::ScrollChanged(const double x, const double y) {

        }

        void OrthoCamera::CursorMoved(const double x, const double y) {
            Look(x, y);
        }

    }

}
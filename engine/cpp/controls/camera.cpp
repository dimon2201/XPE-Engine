#include <controls/camera.h>

namespace xpe {

    namespace control {

        Camera::Camera(CameraBuffer* cameraBuffer)
        : m_CameraBuffer(cameraBuffer) {
            m_ViewWidth = WindowManager::GetWidth();
            m_ViewHeight = WindowManager::GetHeight();
        }

        glm::vec2 Camera::GetPanSpeed() {
            float panAcc = PanAcceleration;
            int width = m_ViewWidth;
            int height = m_ViewHeight;

            float x = math::min(width / 1000.0f, 2.4f);
            float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

            float y = math::min(height / 1000.0f, 2.4f);
            float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

            return { xFactor * panAcc, yFactor * panAcc };
        }

        float Camera::GetZoomSpeed() {
            float distance = MoveSpeed * ZoomAcceleration;
            distance = math::max(distance, 0.0f);
            return distance * distance;
        }

        void Camera::Pan(const glm::vec2 &delta) {
            float distance = MoveSpeed;
            glm::vec3& focalPoint = m_Position;
            glm::vec2 speed = GetPanSpeed();
            focalPoint += -GetRightDirection() * delta.x * speed.x * distance;
            focalPoint += GetUpDirection() * delta.y * speed.y * distance;
        }

        PerspectiveCamera::PerspectiveCamera(
            CameraBuffer* cameraBuffer,
            PerspectiveCameraComponent* component
        ) : Camera(cameraBuffer),
            Component(component),
            MaxFovDegree(component->Projection.FovDegree)
        {
            auto& buffer = *m_CameraBuffer;
            auto& componentData = *component;
            auto& bufferData = *buffer.GetItem(componentData.Index);

            math::ViewMatrix viewMatrix;
            viewMatrix.Up = componentData.Up;
            viewMatrix.Front = componentData.Front;
            viewMatrix.Position = componentData.Position;

            auto& projection = Component->Projection;
            projection.AspectRatio = m_ViewWidth / m_ViewHeight;

            bufferData.Position = componentData.Position;
            bufferData.View = math::ViewMatrixUpdate(viewMatrix);
            bufferData.Projection = math::PerspectiveMatrixUpdate(projection);
            buffer.Flush();

            Input::WindowFrameResizedEvents.AddEvent(this, core::OnWindowFrameResized<PerspectiveCamera>, 2);
            Input::ScrollChangedEvents.AddEvent(this, core::OnScrollChanged<PerspectiveCamera>, 2);
            Input::CursorMovedEvents.AddEvent(this, core::OnCursorMoved<PerspectiveCamera>, 2);
        }

        void PerspectiveCamera::Move() {
            auto& component = *Component;
            auto& position = component.Position;
            glm::vec3 front = component.Front;
            glm::vec3 up = component.Up;
            glm::vec3 right = glm::cross(front, up);

            if (Input::KeyPressed(KeyMoveForward)) {
                position = CalculateFrontPosition();
            }

            else if (Input::KeyPressed(KeyMoveLeft)) {
                position = CalculateLeftPosition();
            }

            else if (Input::KeyPressed(KeyMoveBackward)) {
                position = CalculateBackPosition();
            }

            else if (Input::KeyPressed(KeyMoveRight)) {
                position = CalculateRightPosition();
            }

            else
                return;

            UpdateView(position);
            m_CameraBuffer->Flush();
        }

        void PerspectiveCamera::ZoomIn() {
            auto& cameraBuffer = *m_CameraBuffer;
            auto& fov = Component->Projection.FovDegree;

            fov -= (float) GetZoomSpeed();
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            cameraBuffer.GetItem(Component->Index)->Projection = math::PerspectiveMatrixUpdate(Component->Projection);
            cameraBuffer.Flush();
        }

        void PerspectiveCamera::ZoomOut() {
            auto& cameraBuffer = *m_CameraBuffer;
            auto& fov = Component->Projection.FovDegree;

            fov += (float) GetZoomSpeed();
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            cameraBuffer.GetItem(Component->Index)->Projection = math::PerspectiveMatrixUpdate(Component->Projection);
            cameraBuffer.Flush();
        }

        void PerspectiveCamera::ScrollChanged(const double x, const double y) {
            auto& cameraBuffer = *m_CameraBuffer;
            auto& fov = Component->Projection.FovDegree;

            fov -= (float) y * GetZoomSpeed();
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            cameraBuffer.GetItem(Component->Index)->Projection = math::PerspectiveMatrixUpdate(Component->Projection);
            cameraBuffer.Flush();
        }

        void PerspectiveCamera::Look(const double x, const double y) {
            Input::CaptureCursor(x, y);
            auto& cursorDelta = Input::GetMouseCursor().Delta;
            float lookSign = (float) LookMode;
            float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
            Yaw += yawSign * cursorDelta.x * HorizontalSensitivity * lookSign;
            Pitch += cursorDelta.y * VerticalSensitivity * lookSign;
            UpdateView(m_Position);
            m_CameraBuffer->Flush();
        }

        void PerspectiveCamera::WindowFrameResized(int w, int h) {
            m_ViewWidth = w;
            m_ViewHeight = h;
            UpdateProjection();
            m_CameraBuffer->Flush();
        }

        void PerspectiveCamera::CursorMoved(const double x, const double y) {
            Look(x, y);
        }

        void PerspectiveCamera::UpdateProjection()
        {
            auto& component = *Component;
            auto& cameraBuffer = *m_CameraBuffer;
            auto& cameraBufferData = *cameraBuffer.GetItem(component.Index);
            auto& projection = component.Projection;

            projection.AspectRatio = m_ViewWidth / m_ViewHeight;

            cameraBufferData.Projection = math::PerspectiveMatrixUpdate(projection);
        }

        void PerspectiveCamera::UpdateView(const glm::vec3& position)
        {
            // m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
            auto& component = *Component;
            auto& cameraBuffer = *m_CameraBuffer;
            auto& cameraBufferData = *cameraBuffer.GetItem(component.Index);

            glm::quat orientation = GetOrientation();

            glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(orientation);
            viewMatrix = glm::inverse(viewMatrix);

            component.Position = position;
            m_Position = position;
            cameraBufferData.Position = position;
            cameraBufferData.View = viewMatrix;
        }

        OrthoCamera::OrthoCamera(
            CameraBuffer *cameraBuffer,
            OrthoCameraComponent *component
        ) : Camera(cameraBuffer), Component(component) {

            auto& buffer = *m_CameraBuffer;
            auto& componentData = *component;
            auto& bufferData = *buffer.GetItem(componentData.Index);

            math::ViewMatrix viewMatrix;
            viewMatrix.Position = componentData.Position;
            viewMatrix.Front = componentData.Front;
            viewMatrix.Up = componentData.Up;

            auto& projection = componentData.Projection;
            projection.Right = m_ViewWidth / 100.0f;
            projection.Top = m_ViewHeight / 100.0f;

            bufferData.Position = componentData.Position;
            bufferData.View = math::ViewMatrixUpdate(viewMatrix);
            bufferData.Projection = math::OrthoMatrixUpdate(projection);
            buffer.Flush();

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

        void OrthoCamera::WindowFrameResized(int width, int height) {
            auto& component = *Component;
            auto& projection = component.Projection;
            auto& cameraBuffer = *m_CameraBuffer;

            m_ViewWidth = width;
            m_ViewHeight = height;

            projection.Right = width / 100.0f;
            projection.Top = height / 100.0f;

            cameraBuffer.GetItem(component.Index)->Projection = math::OrthoMatrixUpdate(projection);
            cameraBuffer.Flush();
        }

        void OrthoCamera::ScrollChanged(const double x, const double y) {
        }

        void OrthoCamera::CursorMoved(const double x, const double y) {
            Look(x, y);
        }

    }

}
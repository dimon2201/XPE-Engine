#include <rendering/camera.h>

namespace xpe {

    namespace render {

        glm::vec2 Camera::GetPanSpeed()
        {
            float panAcc = PanAcceleration;
            int width = m_ViewWidth;
            int height = m_ViewHeight;

            float x = math::min(width / 1000.0f, 2.4f);
            float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

            float y = math::min(height / 1000.0f, 2.4f);
            float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

            return { xFactor * panAcc, yFactor * panAcc };
        }

        float Camera::GetZoomSpeed()
        {
            float distance = MoveSpeed * ZoomAcceleration;
            distance = math::max(distance, 0.0f);
            return distance * distance;
        }

        PerspectiveCamera::PerspectiveCamera(int viewWidth, int viewHeight, CameraBuffer* buffer)
        : Camera(buffer)
        {
            m_ViewWidth = viewWidth;
            m_ViewHeight = viewHeight;
            AddWindowFrameResized(PerspectiveCamera, 2);
            AddCursorMove(PerspectiveCamera, 2);
            AddScrollChanged(PerspectiveCamera, 2);
        }

        PerspectiveCamera::~PerspectiveCamera()
        {
            RemoveWindowFrameResized();
            RemoveCursorMove();
            RemoveScrollChanged();
        }

        void PerspectiveCamera::Flush()
        {
            MaxFovDegree = Component.FovDegree;

            math::ViewMatrix viewMatrix;
            viewMatrix.Up = Component.Up;
            viewMatrix.Front = Component.Front;
            viewMatrix.Position = Component.Position;

            Component.AspectRatio = m_ViewWidth / m_ViewHeight;

            auto& bufferData = m_Buffer->Item;
            bufferData.Position = Component.Position;
            bufferData.View = math::ViewMatrixUpdate(viewMatrix);
            bufferData.Projection = math::PerspectiveMatrixUpdate(Component);
            m_Buffer->Flush();
        }

        void PerspectiveCamera::Pan(const glm::vec2 &delta)
        {
            float distance = MoveSpeed;
            glm::vec3& focalPoint = m_Position;
            glm::vec2 speed = GetPanSpeed();

            focalPoint += -GetRightDirection() * delta.x * speed.x * distance;
            focalPoint += GetUpDirection() * delta.y * speed.y * distance;

            UpdateView(focalPoint);
            m_Buffer->Flush();
        }

        void PerspectiveCamera::Move()
        {
            auto& position = Component.Position;
            glm::vec3 front = Component.Front;
            glm::vec3 up = Component.Up;
            glm::vec3 right = glm::cross(front, up);

            if (InputManager::KeyPressed(KeyMoveForward))
            {
                position = CalculateFrontPosition();
            }

            else if (InputManager::KeyPressed(KeyMoveLeft))
            {
                position = CalculateLeftPosition();
            }

            else if (InputManager::KeyPressed(KeyMoveBackward))
            {
                position = CalculateBackPosition();
            }

            else if (InputManager::KeyPressed(KeyMoveRight))
            {
                position = CalculateRightPosition();
            }

            else
                return;

            UpdateView(position);
            m_Buffer->Flush();
        }

        void PerspectiveCamera::ZoomIn()
        {
            auto& cameraBuffer = *m_Buffer;
            auto& fov = Component.FovDegree;

            fov -= (float) GetZoomSpeed();
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            cameraBuffer.Item.Projection = math::PerspectiveMatrixUpdate(Component);
            cameraBuffer.Flush();
        }

        void PerspectiveCamera::ZoomOut()
        {
            auto& cameraBuffer = *m_Buffer;
            auto& fov = Component.FovDegree;

            fov += (float) GetZoomSpeed();
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            cameraBuffer.Item.Projection = math::PerspectiveMatrixUpdate(Component);
            cameraBuffer.Flush();
        }

        void PerspectiveCamera::ScrollChanged(const double x, const double y)
        {
            auto& cameraBuffer = *m_Buffer;
            auto& fov = Component.FovDegree;

            fov -= (float) y * GetZoomSpeed();
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            cameraBuffer.Item.Projection = math::PerspectiveMatrixUpdate(Component);
            cameraBuffer.Flush();
        }

        void PerspectiveCamera::Look(const double x, const double y)
        {
            if (!EnableLook)
                return;

            InputManager::CaptureCursor(x, y);
            auto& cursorDelta = InputManager::GetMouseCursor().Delta;
            float lookSign = (float) LookMode;
            float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;

            Yaw += yawSign * cursorDelta.x * HorizontalSensitivity * lookSign;
            Pitch += cursorDelta.y * VerticalSensitivity * lookSign;

            UpdateView(m_Position);
            m_Buffer->Flush();
        }

        void PerspectiveCamera::WindowFrameResized(int w, int h)
        {
            LogInfo("PerspectiveCamera::WindowFrameResized: width={}, height={}", w, h);
            m_ViewWidth = w;
            m_ViewHeight = h;
            UpdateProjection();
            m_Buffer->Flush();
        }

        void PerspectiveCamera::CursorMoved(const double x, const double y)
        {
            Look(x, y);
        }

        void PerspectiveCamera::UpdateProjection()
        {
            Component.AspectRatio = m_ViewWidth / m_ViewHeight;
            m_Buffer->Item.Projection = math::PerspectiveMatrixUpdate(Component);
        }

        void PerspectiveCamera::UpdateView(const glm::vec3& position)
        {
            // m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
            glm::quat orientation = GetOrientation();

            glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(orientation);
            viewMatrix = glm::inverse(viewMatrix);

            Component.Position = position;
            m_Position = position;
            m_Buffer->Item.Position = position;
            m_Buffer->Item.View = viewMatrix;
        }

        OrthoCamera::OrthoCamera(int viewWidth, int viewHeight, CameraBuffer* buffer) : Camera(buffer)
        {
            m_ViewWidth = viewWidth;
            m_ViewHeight = viewHeight;
            AddWindowFrameResized(OrthoCamera, 2);
            AddScrollChanged(OrthoCamera, 2);
            AddCursorMove(OrthoCamera, 2);
        }

        OrthoCamera::~OrthoCamera()
        {
            RemoveWindowFrameResized();
            RemoveScrollChanged();
            RemoveCursorMove();
        }

        void OrthoCamera::Flush()
        {
            math::ViewMatrix viewMatrix;
            viewMatrix.Position = Component.Position;
            viewMatrix.Front = Component.Front;
            viewMatrix.Up = Component.Up;

            Component.Right = m_ViewWidth;
            Component.Left = -m_ViewWidth;
            Component.Top = m_ViewHeight;
            Component.Bottom = -m_ViewHeight;

            auto& bufferData = m_Buffer->Item;
            bufferData.Position = Component.Position;
            bufferData.View = math::ViewMatrixUpdate(viewMatrix);
            bufferData.Projection = math::OrthoMatrixUpdate(Component);
            m_Buffer->Flush();
        }

        void OrthoCamera::Move()
        {
            // todo ortho camera move function
        }

        void OrthoCamera::ZoomIn()
        {
            // todo ortho camera zoom in function
        }

        void OrthoCamera::ZoomOut()
        {
            // todo ortho camera zoom out function
        }

        void OrthoCamera::Look(const double x, const double y)
        {
            // todo ortho camera look function
        }

        void OrthoCamera::WindowFrameResized(int width, int height)
        {
            LogInfo("OrthoCamera::WindowFrameResized: width={}, height={}", width, height);
            m_ViewWidth = width;
            m_ViewHeight = height;
            UpdateProjection();
            m_Buffer->Flush();
        }

        void OrthoCamera::ScrollChanged(const double x, const double y)
        {
        }

        void OrthoCamera::CursorMoved(const double x, const double y)
        {
            Look(x, y);
        }

        void OrthoCamera::UpdateProjection()
        {
            Component.Right = m_ViewWidth;
            Component.Left = -m_ViewWidth;
            Component.Top = m_ViewHeight;
            Component.Bottom = -m_ViewHeight;
            m_Buffer->Item.Projection = math::OrthoMatrixUpdate(Component);
        }

    }

}
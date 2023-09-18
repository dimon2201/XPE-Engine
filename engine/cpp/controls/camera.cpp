#include <controls/camera.h>

namespace xpe {

    namespace control {

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

        void PerspectiveCamera::Init(int viewWidth, int viewHeight)
        {
            m_ViewWidth = viewWidth;
            m_ViewHeight = viewHeight;

            MaxFovDegree = Component.Projection.FovDegree;

            auto& bufferData = Buffer->Item;

            math::ViewMatrix viewMatrix;
            viewMatrix.Up = Component.Up;
            viewMatrix.Front = Component.Front;
            viewMatrix.Position = Component.Position;

            auto& projection = Component.Projection;
            projection.AspectRatio = m_ViewWidth / m_ViewHeight;

            bufferData.Position = Component.Position;
            bufferData.View = math::ViewMatrixUpdate(viewMatrix);
            bufferData.Projection = math::PerspectiveMatrixUpdate(projection);
            Buffer->Flush();

            AddWindowFrameResized(PerspectiveCamera, 2);
            AddCursorMove(PerspectiveCamera, 2);
            AddScrollChanged(PerspectiveCamera, 2);
        }

        void PerspectiveCamera::Pan(const glm::vec2 &delta)
        {
            float distance = MoveSpeed;
            glm::vec3& focalPoint = m_Position;
            glm::vec2 speed = GetPanSpeed();

            focalPoint += -GetRightDirection() * delta.x * speed.x * distance;
            focalPoint += GetUpDirection() * delta.y * speed.y * distance;

            UpdateView(focalPoint);
            Buffer->Flush();
        }

        void PerspectiveCamera::Move()
        {
            auto& position = Component.Position;
            glm::vec3 front = Component.Front;
            glm::vec3 up = Component.Up;
            glm::vec3 right = glm::cross(front, up);

            if (Input::KeyPressed(KeyMoveForward))
            {
                position = CalculateFrontPosition();
            }

            else if (Input::KeyPressed(KeyMoveLeft))
            {
                position = CalculateLeftPosition();
            }

            else if (Input::KeyPressed(KeyMoveBackward))
            {
                position = CalculateBackPosition();
            }

            else if (Input::KeyPressed(KeyMoveRight))
            {
                position = CalculateRightPosition();
            }

            else
                return;

            UpdateView(position);
            Buffer->Flush();
        }

        void PerspectiveCamera::ZoomIn()
        {
            auto& cameraBuffer = *Buffer;
            auto& fov = Component.Projection.FovDegree;

            fov -= (float) GetZoomSpeed();
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            cameraBuffer.Item.Projection = math::PerspectiveMatrixUpdate(Component.Projection);
            cameraBuffer.Flush();
        }

        void PerspectiveCamera::ZoomOut()
        {
            auto& cameraBuffer = *Buffer;
            auto& fov = Component.Projection.FovDegree;

            fov += (float) GetZoomSpeed();
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            cameraBuffer.Item.Projection = math::PerspectiveMatrixUpdate(Component.Projection);
            cameraBuffer.Flush();
        }

        void PerspectiveCamera::ScrollChanged(const double x, const double y)
        {
            auto& cameraBuffer = *Buffer;
            auto& fov = Component.Projection.FovDegree;

            fov -= (float) y * GetZoomSpeed();
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            cameraBuffer.Item.Projection = math::PerspectiveMatrixUpdate(Component.Projection);
            cameraBuffer.Flush();
        }

        void PerspectiveCamera::Look(const double x, const double y)
        {
            if (!EnableLook)
                return;

            Input::CaptureCursor(x, y);
            auto& cursorDelta = Input::GetMouseCursor().Delta;
            float lookSign = (float) LookMode;
            float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;

            Yaw += yawSign * cursorDelta.x * HorizontalSensitivity * lookSign;
            Pitch += cursorDelta.y * VerticalSensitivity * lookSign;

            UpdateView(m_Position);
            Buffer->Flush();
        }

        void PerspectiveCamera::WindowFrameResized(int w, int h)
        {
            LogInfo("WindowFrameResized: width={}, height={}", w, h);
            m_ViewWidth = w;
            m_ViewHeight = h;
            UpdateProjection();
            Buffer->Flush();
        }

        void PerspectiveCamera::CursorMoved(const double x, const double y)
        {
            Look(x, y);
        }

        void PerspectiveCamera::UpdateProjection()
        {
            auto& cameraBuffer = *Buffer;
            auto& cameraBufferData = cameraBuffer.Item;
            auto& projection = Component.Projection;

            projection.AspectRatio = m_ViewWidth / m_ViewHeight;

            cameraBufferData.Projection = math::PerspectiveMatrixUpdate(projection);
        }

        void PerspectiveCamera::UpdateView(const glm::vec3& position)
        {
            // m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
            auto& cameraBuffer = *Buffer;
            auto& cameraBufferData = cameraBuffer.Item;

            glm::quat orientation = GetOrientation();

            glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(orientation);
            viewMatrix = glm::inverse(viewMatrix);

            Component.Position = position;
            m_Position = position;
            cameraBufferData.Position = position;
            cameraBufferData.View = viewMatrix;
        }

        void OrthoCamera::Init(int viewWidth, int viewHeight)
        {
            m_ViewWidth = viewWidth;
            m_ViewHeight = viewHeight;

            auto& buffer = *Buffer;
            auto& bufferData = buffer.Item;

            math::ViewMatrix viewMatrix;
            viewMatrix.Position = Component.Position;
            viewMatrix.Front = Component.Front;
            viewMatrix.Up = Component.Up;

            auto& projection = Component.Projection;
            projection.Right = m_ViewWidth;
            projection.Left = -m_ViewWidth;
            projection.Top = m_ViewHeight;
            projection.Bottom = -m_ViewHeight;

            bufferData.Position = Component.Position;
            bufferData.View = math::ViewMatrixUpdate(viewMatrix);
            bufferData.Projection = math::OrthoMatrixUpdate(projection);
            buffer.Flush();

            AddWindowFrameResized(OrthoCamera, 2);
            AddScrollChanged(OrthoCamera, 2);
            AddCursorMove(OrthoCamera, 2);
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
            m_ViewWidth = width;
            m_ViewHeight = height;
            UpdateProjection();
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
            auto& projection = Component.Projection;
            auto& cameraBuffer = *Buffer;

            projection.Right = m_ViewWidth;
            projection.Left = -m_ViewWidth;
            projection.Top = m_ViewHeight;
            projection.Bottom = -m_ViewHeight;

            cameraBuffer.Item.Projection = math::OrthoMatrixUpdate(projection);
            cameraBuffer.Flush();
        }

    }

}
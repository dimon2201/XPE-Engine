#include <rendering/camera.hpp>
#include <rendering/bindings.hpp>

namespace xpe {

    namespace render {

        glm::vec2 cCamera::GetPanSpeed()
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

        float cCamera::GetZoomSpeed()
        {
            float distance = MoveSpeed * ZoomAcceleration;
            distance = math::max(distance, 0.0f);
            return distance * distance;
        }

        cPerspectiveCamera::cPerspectiveCamera(int viewWidth, int viewHeight)
        {
            m_ViewWidth = viewWidth;
            m_ViewHeight = viewHeight;
            m_Position = Component.Position;
            AddWindowResized(cPerspectiveCamera, eWindowFrameResizedPriority::CAMERA);
            AddWindowRatioChanged(cPerspectiveCamera, 1);
            AddCursorMove(cPerspectiveCamera, 2);
            AddScrollChanged(cPerspectiveCamera, 2);
        }

        cPerspectiveCamera::~cPerspectiveCamera()
        {
            RemoveWindowFrameResized();
            RemoveWindowRatioChanged();
            RemoveCursorMove();
            RemoveScrollChanged();
        }

        void cPerspectiveCamera::Flush()
        {
            m_Position = Component.Position;
            MaxFovDegree = Component.FovDegree;

            math::sViewMatrix viewMatrix;
            viewMatrix.Position = Component.Position;
            viewMatrix.Front = Component.Front;
            viewMatrix.Up = Component.Up;

            auto& bufferData = Buffers::Camera->Item;
            bufferData.Position = Component.Position;
            bufferData.View = cMathManager::UpdateViewMatrix(viewMatrix);
            bufferData.Projection = cMathManager::UpdatePerspectiveMatrix(Component);
            Buffers::Camera->Flush();
        }

        void cPerspectiveCamera::Pan(const glm::vec2 &delta)
        {
            float distance = MoveSpeed;
            glm::vec3& focalPoint = m_Position;
            glm::vec2 speed = GetPanSpeed();

            focalPoint += -GetRightDirection() * delta.x * speed.x * distance;
            focalPoint += GetUpDirection() * delta.y * speed.y * distance;

            UpdateView(focalPoint);
            Buffers::Camera->Flush();
        }

        void cPerspectiveCamera::Move()
        {
            auto& position = Component.Position;
            glm::vec3 front = Component.Front;
            glm::vec3 up = Component.Up;
            glm::vec3 right = glm::cross(front, up);

            if (cInputManager::KeyPressed(KeyMoveForward))
            {
                position = CalculateFrontPosition();
            }

            else if (cInputManager::KeyPressed(KeyMoveLeft))
            {
                position = CalculateLeftPosition();
            }

            else if (cInputManager::KeyPressed(KeyMoveBackward))
            {
                position = CalculateBackPosition();
            }

            else if (cInputManager::KeyPressed(KeyMoveRight))
            {
                position = CalculateRightPosition();
            }

            else
                return;

            UpdateView(position);
            Buffers::Camera->Flush();
        }

        void cPerspectiveCamera::ZoomIn()
        {
            auto& cameraBuffer = *Buffers::Camera;
            auto& fov = Component.FovDegree;

            fov -= (float) GetZoomSpeed();
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            cameraBuffer.Item.Projection = cMathManager::UpdatePerspectiveMatrix(Component);
            cameraBuffer.Flush();
        }

        void cPerspectiveCamera::ZoomOut()
        {
            auto& cameraBuffer = *Buffers::Camera;
            auto& fov = Component.FovDegree;

            fov += (float) GetZoomSpeed();
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            cameraBuffer.Item.Projection = cMathManager::UpdatePerspectiveMatrix(Component);
            cameraBuffer.Flush();
        }

        void cPerspectiveCamera::ScrollChanged(const double x, const double y)
        {
            auto& cameraBuffer = *Buffers::Camera;
            auto& fov = Component.FovDegree;

            fov -= (float) y * GetZoomSpeed();
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            cameraBuffer.Item.Projection = cMathManager::UpdatePerspectiveMatrix(Component);
            cameraBuffer.Flush();
        }

        void cPerspectiveCamera::Look(const double x, const double y)
        {
            if (!EnableLook)
                return;

            cInputManager::CaptureCursor(x, y);
            auto& cursorDelta = cInputManager::GetMouseCursor().Delta;
            float lookSign = (float) LookMode;
            float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;

            Yaw += yawSign * cursorDelta.x * HorizontalSensitivity * lookSign;
            Pitch += cursorDelta.y * VerticalSensitivity * lookSign;

            UpdateView(m_Position);
            Buffers::Camera->Flush();
        }

        void cPerspectiveCamera::WindowResized(int w, int h)
        {
            LogInfo("cPerspectiveCamera::WindowResized: width={}, height={}", w, h);
            m_ViewWidth = w;
            m_ViewHeight = h;
            WindowRatioChanged((float) w / (float) h);
        }

        void cPerspectiveCamera::WindowRatioChanged(float ratio)
        {
            LogInfo("cPerspectiveCamera::WindowRatioChanged: ratio={}", ratio);
            Component.AspectRatio = ratio;
            UpdateProjection();
            Buffers::Camera->Flush();
        }

        void cPerspectiveCamera::CursorMoved(const double x, const double y)
        {
            Look(x, y);
        }

        void cPerspectiveCamera::UpdateProjection()
        {
            Buffers::Camera->Item.Projection = cMathManager::UpdatePerspectiveMatrix(Component);
        }

        void cPerspectiveCamera::UpdateView(const glm::vec3& position)
        {
            // m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
            glm::quat orientation = GetOrientation();

            glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(orientation);
            viewMatrix = glm::inverse(viewMatrix);

            Component.Position = position;
            m_Position = position;
            Buffers::Camera->Item.Position = position;
            Buffers::Camera->Item.View = viewMatrix;
        }

        cOrthoCamera::cOrthoCamera(int viewWidth, int viewHeight)
        {
            m_ViewWidth = viewWidth;
            m_ViewHeight = viewHeight;
            m_Position = Component.Position;
            AddWindowResized(cOrthoCamera, eWindowFrameResizedPriority::CAMERA);
            AddScrollChanged(cOrthoCamera, 2);
            AddCursorMove(cOrthoCamera, 2);
        }

        cOrthoCamera::~cOrthoCamera()
        {
            RemoveWindowFrameResized();
            RemoveScrollChanged();
            RemoveCursorMove();
        }

        void cOrthoCamera::Flush()
        {
            m_Position = Component.Position;

            math::sViewMatrix viewMatrix;
            viewMatrix.Position = Component.Position;
            viewMatrix.Front = Component.Front;
            viewMatrix.Up = Component.Up;

            Component.Right = m_ViewWidth;
            Component.Left = -m_ViewWidth;
            Component.Top = m_ViewHeight;
            Component.Bottom = -m_ViewHeight;

            auto& bufferData = Buffers::Camera->Item;
            bufferData.Position = Component.Position;
            bufferData.View = cMathManager::UpdateViewMatrix(viewMatrix);
            bufferData.Projection = cMathManager::UpdateOrthoMatrix(Component);
            Buffers::Camera->Flush();
        }

        void cOrthoCamera::Move()
        {
            // todo ortho camera move function
        }

        void cOrthoCamera::ZoomIn()
        {
            // todo ortho camera zoom in function
        }

        void cOrthoCamera::ZoomOut()
        {
            // todo ortho camera zoom out function
        }

        void cOrthoCamera::Look(const double x, const double y)
        {
            // todo ortho camera look function
        }

        void cOrthoCamera::WindowResized(int width, int height)
        {
            LogInfo("cOrthoCamera::WindowResized: width={}, height={}", width, height);
            m_ViewWidth = width;
            m_ViewHeight = height;
            UpdateProjection();
            Buffers::Camera->Flush();
        }

        void cOrthoCamera::ScrollChanged(const double x, const double y)
        {
        }

        void cOrthoCamera::CursorMoved(const double x, const double y)
        {
            Look(x, y);
        }

        void cOrthoCamera::UpdateProjection()
        {
            Component.Right = m_ViewWidth;
            Component.Left = -m_ViewWidth;
            Component.Top = m_ViewHeight;
            Component.Bottom = -m_ViewHeight;
            Buffers::Camera->Item.Projection = cMathManager::UpdateOrthoMatrix(Component);
        }

    }

}
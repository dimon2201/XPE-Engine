#include <camera_manager.hpp>

namespace xpe
{
    namespace render
    {
        CPerspectiveCamera* MCamera::Camera = nullptr;
        int MCamera::s_ViewWidth = 0;
        int MCamera::s_ViewHeight = 0;
        glm::vec3 MCamera::s_Position = { 0, 0, 0 };

        void MCamera::Init(int viewWidth, int viewHeight)
        {
            Profile();

            Buffers::Camera = new sCameraBuffer();
            s_ViewWidth = viewWidth;
            s_ViewHeight = viewHeight;
            s_Position = Camera->Position;
            MEvent::WindowFrameResized.Add("Camera", eWindowFrameResizedPriority::CAMERA, [](int w, int h)
            {
                MCamera::WindowFrameResized(w, h);
            });
            MEvent::WindowRatioChanged.Add("Camera", 1, [](float ratio)
            {
                MCamera::WindowRatioChanged(ratio);
            });
            MEvent::CursorMoved.Add("Camera", 2, [](const double x, const double y)
            {
                MCamera::CursorMoved(x, y);
            });
            MEvent::ScrollChanged.Add("Camera", 2, [](const double x, const double y)
            {
                MCamera::ScrollChanged(x, y);
            });
        }

        void MCamera::Free()
        {
            Profile();

            delete Buffers::Camera;
            MEvent::WindowFrameResized.Remove("Camera");
            MEvent::WindowRatioChanged.Remove("Camera");
            MEvent::CursorMoved.Remove("Camera");
            MEvent::ScrollChanged.Remove("Camera");
        }

        glm::vec2 MCamera::GetPanSpeed()
        {
            float panAcc = Camera->PanAcceleration;
            int width = s_ViewWidth;
            int height = s_ViewHeight;

            float x = Min(width / 1000.0f, 2.4f);
            float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

            float y = Min(height / 1000.0f, 2.4f);
            float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

            return { xFactor * panAcc, yFactor * panAcc };
        }

        float MCamera::GetZoomSpeed()
        {
            float distance = Camera->MoveSpeed * Camera->ZoomAcceleration;
            distance = Max(distance, 0.0f);
            return distance * distance;
        }

        void MCamera::Flush()
        {
            Profile();

            s_Position = Camera->Position;
            Camera->MaxFovDegree = Camera->Projection.FovDegree;

            math::sViewMatrix viewMatrix;
            viewMatrix.Position = Camera->Position;
            viewMatrix.Front = Camera->Front;
            viewMatrix.Up = Camera->Up;

            auto& bufferData = Buffers::Camera->Item;
            bufferData.Position = Camera->Position;
            bufferData.View = MMath::UpdateViewMatrix(viewMatrix);
            bufferData.Projection = MMath::UpdatePerspectiveMatrix(Camera->Projection);
            bufferData.Viewport = Camera->Viewport;
            bufferData.Exposure = Camera->Exposure;
            bufferData.Gamma = Camera->Gamma;
            Buffers::Camera->Flush();
        }

        void MCamera::Pan(const glm::vec2 &delta)
        {
            float distance = Camera->MoveSpeed;
            glm::vec3& focalPoint = s_Position;
            glm::vec2 speed = GetPanSpeed();

            focalPoint += -GetRightDirection() * delta.x * speed.x * distance;
            focalPoint += GetUpDirection() * delta.y * speed.y * distance;

            UpdateView(focalPoint);
            Buffers::Camera->Flush();
        }

        void MCamera::Move()
        {
            Profile();

            auto& position = Camera->Position;
            glm::vec3 front = Camera->Front;
            glm::vec3 up = Camera->Up;
            glm::vec3 right = glm::cross(front, up);

            if (MInput::KeyPressed(Camera->KeyMoveForward))
            {
                position = CalculateFrontPosition();
            }

            else if (MInput::KeyPressed(Camera->KeyMoveLeft))
            {
                position = CalculateLeftPosition();
            }

            else if (MInput::KeyPressed(Camera->KeyMoveBackward))
            {
                position = CalculateBackPosition();
            }

            else if (MInput::KeyPressed(Camera->KeyMoveRight))
            {
                position = CalculateRightPosition();
            }

            else
            {
                return;
            }

            UpdateView(position);
            Buffers::Camera->Flush();
        }

        void MCamera::ZoomIn()
        {
            auto& cameraBuffer = *Buffers::Camera;
            auto& fov = Camera->Projection.FovDegree;

            fov -= (float) GetZoomSpeed();
            math::Clamp(fov, Camera->MinFovDegree, Camera->MaxFovDegree);

            cameraBuffer.Item.Projection = MMath::UpdatePerspectiveMatrix(Camera->Projection);
            cameraBuffer.Flush();
        }

        void MCamera::ZoomOut()
        {
            auto& cameraBuffer = *Buffers::Camera;
            auto& fov = Camera->Projection.FovDegree;

            fov += (float) GetZoomSpeed();
            math::Clamp(fov, Camera->MinFovDegree, Camera->MaxFovDegree);

            cameraBuffer.Item.Projection = MMath::UpdatePerspectiveMatrix(Camera->Projection);
            cameraBuffer.Flush();
        }

        void MCamera::ScrollChanged(const double x, const double y)
        {
            auto& cameraBuffer = *Buffers::Camera;
            auto& fov = Camera->Projection.FovDegree;

            fov -= (float) y * GetZoomSpeed();
            math::Clamp(fov, Camera->MinFovDegree, Camera->MaxFovDegree);

            cameraBuffer.Item.Projection = MMath::UpdatePerspectiveMatrix(Camera->Projection);
            cameraBuffer.Flush();
        }

        void MCamera::Look(const double x, const double y)
        {
            if (!Camera->EnableLook)
            {
                return;
            }

            MInput::CaptureCursor(x, y);
            auto& cursorDelta = MInput::GetMouseCursor().Delta;
            float lookSign = (float) Camera->Mode;
            float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;

            Camera->Rotation.y += yawSign * cursorDelta.x * Camera->HorizontalSensitivity * lookSign;
            Camera->Rotation.x += cursorDelta.y * Camera->VerticalSensitivity * lookSign;

            UpdateView(s_Position);
            Buffers::Camera->Flush();
        }

        void MCamera::Resize(int width, int height)
        {
            LogInfo("MCamera::Resize: width={}, height={}", width, height);
            if (Camera != nullptr)
            {
                Camera->Viewport.Width = width;
                Camera->Viewport.Height = height;
                Flush();
            }
        }

        void MCamera::WindowFrameResized(int w, int h)
        {
            LogInfo("MCamera::WindowFrameResized: width={}, height={}", w, h);
            s_ViewWidth = w;
            s_ViewHeight = h;
            WindowRatioChanged((float) w / (float) h);
        }

        void MCamera::WindowRatioChanged(float ratio)
        {
            LogInfo("MCamera::WindowRatioChanged: ratio={}", ratio);
            Camera->Projection.AspectRatio = ratio;
            UpdateProjection();
            Buffers::Camera->Flush();
        }

        void MCamera::CursorMoved(const double x, const double y)
        {
            Look(x, y);
        }

        void MCamera::UpdateProjection()
        {
            Buffers::Camera->Item.Projection = MMath::UpdatePerspectiveMatrix(Camera->Projection);
        }

        void MCamera::UpdateView(const glm::vec3& position)
        {
            // m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
            glm::quat orientation = GetOrientation();

            glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(orientation);
            viewMatrix = glm::inverse(viewMatrix);

            Camera->Position = position;
            s_Position = position;
            Buffers::Camera->Item.Position = position;
            Buffers::Camera->Item.View = viewMatrix;
        }

        glm::vec3 MCamera::GetUpDirection()
        {
            return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        glm::vec3 MCamera::GetRightDirection()
        {
            return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
        }

        glm::vec3 MCamera::GetForwardDirection()
        {
            return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
        }

        glm::vec3 MCamera::CalculateFrontPosition()
        {
            return s_Position - GetForwardDirection() * Camera->MoveSpeed;
        }

        glm::vec3 MCamera::CalculateBackPosition()
        {
            return s_Position + GetForwardDirection() * Camera->MoveSpeed;
        }

        glm::vec3 MCamera::CalculateRightPosition()
        {
            return s_Position + GetRightDirection() * Camera->MoveSpeed;
        }

        glm::vec3 MCamera::CalculateLeftPosition()
        {
            return s_Position - GetRightDirection() * Camera->MoveSpeed;
        }

        glm::vec3 MCamera::CalculateUpPosition()
        {
            return s_Position + GetUpDirection() * Camera->MoveSpeed;
        }

        glm::vec3 MCamera::CalculateDownPosition()
        {
            return s_Position - GetUpDirection() * Camera->MoveSpeed;
        }

        glm::quat MCamera::GetOrientation()
        {
            return glm::quat(glm::vec3(-Camera->Rotation.x, -Camera->Rotation.y, Camera->Rotation.z));
        }
    }
}
#pragma once

#include <core/user_input.hpp>

#include <rendering/buffers/camera_buffer.h>

namespace xpe {

    namespace control {

        using namespace xpe::core;
        using namespace xpe::render;
        using namespace xpe::ecs;

        class ENGINE_API Camera : public Object {

        public:

            enum class eLookMode {
                EDITOR = 1,
                GAME = -1,
            };

        public:
            eKey KeyMoveForward = eKey::W;
            eKey KeyMoveLeft = eKey::A;
            eKey KeyMoveBackward = eKey::S;
            eKey KeyMoveRight = eKey::D;

            float MoveSpeed = 0.1f;
            float ZoomAcceleration = 10.0f;
            float PanAcceleration = 10.0f;
            float HorizontalSensitivity = 0.01f;
            float VerticalSensitivity = 0.01f;

            float Pitch = 0;
            float Yaw = 0;
            float Roll = 0;

            eLookMode LookMode = eLookMode::GAME;

            bool EnableLook = true;

            glm::vec2 GetPanSpeed();
            float GetZoomSpeed();

        public:
            Camera(CameraBuffer* cameraBuffer);
            ~Camera() = default;

        protected:

            inline glm::vec3 GetUpDirection() const
            {
                return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
            }

            inline glm::vec3 GetRightDirection() const
            {
                return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
            }

            inline glm::vec3 GetForwardDirection() const
            {
                return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
            }

            inline glm::vec3 CalculateFrontPosition() const
            {
                return m_Position - GetForwardDirection() * MoveSpeed;
            }

            inline glm::vec3 CalculateBackPosition() const
            {
                return m_Position + GetForwardDirection() * MoveSpeed;
            }

            inline glm::vec3 CalculateRightPosition() const
            {
                return m_Position + GetRightDirection() * MoveSpeed;
            }

            inline glm::vec3 CalculateLeftPosition() const
            {
                return m_Position - GetRightDirection() * MoveSpeed;
            }

            inline glm::vec3 CalculateUpPosition() const
            {
                return m_Position + GetUpDirection() * MoveSpeed;
            }

            inline glm::vec3 CalculateDownPosition() const
            {
                return m_Position - GetUpDirection() * MoveSpeed;
            }

            inline glm::quat GetOrientation() const
            {
                return glm::quat(glm::vec3(-Pitch, -Yaw, Roll));
            }

        protected:
            CameraBuffer* m_CameraBuffer = nullptr;
            int m_ViewWidth = 0;
            int m_ViewHeight = 0;
            glm::vec3 m_Position = { 0, 0, 0 };
        };

        class ENGINE_API PerspectiveCamera : public Camera {

        public:
            PerspectiveCameraComponent* Component = nullptr;
            float MaxFovDegree = 45.0f;
            float MinFovDegree = 1.0f;

        public:
            PerspectiveCamera(CameraBuffer* cameraBuffer, PerspectiveCameraComponent* component);

            void Move();

            void Pan(const glm::vec2& delta);

            void ZoomIn();

            void ZoomOut();

            void Look(const double x, const double y);

            void ScrollChanged(const double x, const double y);

            void WindowFrameResized(int width, int height);

            void CursorMoved(const double x, const double y);

            void UpdateGamma(float gamma);

            void UpdateExposure(float exposure);

        private:
            void UpdateProjection();
            void UpdateView(const glm::vec3& position);

        };

        class ENGINE_API OrthoCamera : public Camera {

        public:
            OrthoCameraComponent* Component = nullptr;

        public:
            OrthoCamera(CameraBuffer* cameraBuffer, OrthoCameraComponent* component);

            void Move();

            void ZoomIn();
            void ZoomOut();

            void Look(const double x, const double y);

            void WindowFrameResized(int w, int h);

            void ScrollChanged(const double x, const double y);

            void CursorMoved(const double x, const double y);

        private:
            void UpdateProjection();

        };

    }

}
#pragma once

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace ecs;
        using namespace res;

        class ENGINE_API cCamera : public cJson
        {

        public:
            JSON_CLASS(
                cCamera,
                KeyMoveForward,
                KeyMoveLeft,
                KeyMoveBackward,
                KeyMoveRight,
                MoveSpeed,
                ZoomAcceleration,
                PanAcceleration,
                HorizontalSensitivity,
                VerticalSensitivity,
                Pitch,
                Yaw,
                Roll,
                LookMode,
                EnableLook,
                m_ViewWidth,
                m_ViewHeight,
                m_Position
            )

            enum class eLookMode
            {
                EDITOR = 1,
                GAME = -1,
            };

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

            int m_ViewWidth = 0;
            int m_ViewHeight = 0;
            glm::vec3 m_Position = { 0, 0, 0 };
        };

        JSON_ENUM(cCamera::eLookMode, {
            { cCamera::eLookMode::GAME,   "GAME" },
            { cCamera::eLookMode::EDITOR, "EDITOR" }
        })

        class ENGINE_API cPerspectiveCamera : public cCamera {

        public:
            cPerspectiveCamera(int viewWidth, int viewHeight);
            ~cPerspectiveCamera();

            JSON_CLASS(
                cPerspectiveCamera,
                KeyMoveForward,
                KeyMoveLeft,
                KeyMoveBackward,
                KeyMoveRight,
                MoveSpeed,
                ZoomAcceleration,
                PanAcceleration,
                HorizontalSensitivity,
                VerticalSensitivity,
                Pitch,
                Yaw,
                Roll,
                LookMode,
                EnableLook,
                m_ViewWidth,
                m_ViewHeight,
                m_Position,
                MaxFovDegree,
                MinFovDegree,
                Component
            )

            float MaxFovDegree = 45.0f;
            float MinFovDegree = 1.0f;
            CPerspectiveCamera Component;

            void Move();
            void Pan(const glm::vec2& delta);
            void ZoomIn();
            void ZoomOut();
            void Look(const double x, const double y);

            void ScrollChanged(const double x, const double y);
            void WindowResized(int width, int height);
            void WindowRatioChanged(float ratio);
            void CursorMoved(const double x, const double y);

            void Flush();

        private:
            void UpdateProjection();
            void UpdateView(const glm::vec3& position); 

        };

        class ENGINE_API cOrthoCamera : public cCamera {

        public:
            cOrthoCamera(int viewWidth, int viewHeight);
            ~cOrthoCamera();

            JSON_CLASS(
                cOrthoCamera,
                KeyMoveForward,
                KeyMoveLeft,
                KeyMoveBackward,
                KeyMoveRight,
                MoveSpeed,
                ZoomAcceleration,
                PanAcceleration,
                HorizontalSensitivity,
                VerticalSensitivity,
                Pitch,
                Yaw,
                Roll,
                LookMode,
                EnableLook,
                Component
            )

            COrthoCamera Component;

            void Move();
            void ZoomIn();
            void ZoomOut();
            void Look(const double x, const double y);

            void WindowResized(int w, int h);
            void ScrollChanged(const double x, const double y);
            void CursorMoved(const double x, const double y);

            void Flush();

        private:
            void UpdateProjection();

        };

    }

}
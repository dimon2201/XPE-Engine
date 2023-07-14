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
            eKey KeyMoveForward = eKey::W;
            eKey KeyMoveLeft = eKey::A;
            eKey KeyMoveBackward = eKey::S;
            eKey KeyMoveRight = eKey::D;

            float MoveSpeed = 1.0f;
            float ZoomSpeed = 5.0f;

            float HorizontalSensitivity = 0.5f;
            float VerticalSensitivity = 0.5f;

        public:
            Camera(CameraBuffer* cameraBuffer, Time* time);
            ~Camera() = default;

        protected:
            CameraBuffer* m_CameraBuffer = nullptr;
            Time* m_Dt = nullptr;
        };

        class ENGINE_API PerspectiveCamera : public Camera {

        public:
            PerspectiveCameraComponent* Component = nullptr;
            float MaxFovDegree = 45.0f;
            float MinFovDegree = 1.0f;

        public:
            PerspectiveCamera(CameraBuffer* cameraBuffer, PerspectiveCameraComponent* component, Time* time);

            void Move();

            void ZoomIn();

            void ZoomOut();

            void Look(const double x, const double y);

            void ScrollChanged(const double x, const double y);

            void WindowFrameResized(int width, int height);

            void CursorMoved(const double x, const double y);

        };

        class ENGINE_API OrthoCamera : public Camera {

        public:
            OrthoCameraComponent* Component = nullptr;

        public:
            OrthoCamera(CameraBuffer* cameraBuffer, OrthoCameraComponent* component, Time* time);

            void Move();

            void ZoomIn();

            void ZoomOut();

            void Look(const double x, const double y);

            void WindowFrameResized(int w, int h);

            void ScrollChanged(const double x, const double y);

            void CursorMoved(const double x, const double y);

        };

    }

}
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

            float MoveSpeed = 0.01f;
            float ZoomSpeed = 1.0f;
            float PanSpeed = 0.01f;
            float HorizontalSensitivity = 0.01f;
            float VerticalSensitivity = 0.01f;

        public:
            Camera(CameraBuffer* cameraBuffer);
            ~Camera() = default;

        protected:
            CameraBuffer* m_CameraBuffer = nullptr;
        };

        class ENGINE_API PerspectiveCamera : public Camera {

        public:
            PerspectiveCameraComponent* Component = nullptr;
            float MaxFovDegree = 45.0f;
            float MinFovDegree = 1.0f;

        public:
            PerspectiveCamera(CameraBuffer* cameraBuffer, PerspectiveCameraComponent* component);

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
            OrthoCamera(CameraBuffer* cameraBuffer, OrthoCameraComponent* component);

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
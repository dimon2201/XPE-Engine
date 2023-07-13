#pragma once

#include <core/user_input.hpp>

#include <rendering/structure_buffer.h>

namespace xpe {

    namespace control {

        using namespace xpe::core;
        using namespace xpe::render;

        struct ENGINE_API CameraComponent : public Component
        {

            CameraComponent(const string& usid) : Component(usid)
            {}

            u32 Index = 0;

            glm::vec3 Position = { 0, 0, 10 };
            glm::vec3 Front = { 0, 0, -1 };
            glm::vec3 Up = { 0, 1, 0 };

            float Pitch = 0;
            float Yaw = 0;
            float Roll = 0;

            float Distance = 0;
            glm::vec3 FocalPoint = { 0, 0, 0 };

            glm::vec3 GetUpDirection() const;
            glm::vec3 GetRightDirection() const;
            glm::vec3 GetForwardDirection() const;
            glm::vec3 CalculatePosition() const;
            glm::quat GetOrientation() const;

        };

        struct ENGINE_API PerspectiveCameraComponent : public CameraComponent
        {
            PerspectiveCameraComponent(const string& usid) : CameraComponent(usid)
            {}

            math::PerspectiveMatrix Projection;
        };

        struct ENGINE_API OrthoCameraComponent : public CameraComponent
        {
            OrthoCameraComponent(const string& usid) : CameraComponent(usid)
            {}

            math::OrthoMatrix Projection;
        };

        struct ENGINE_API CameraBufferData final {
            glm::mat4 Projection;
            glm::mat4 View;
            glm::vec3 Position;
        };

        class ENGINE_API CameraBuffer : public render::StructureBuffer<CameraBufferData> {

        public:
            CameraBuffer() = default;
            CameraBuffer(Context* context, usize size) : render::StructureBuffer<CameraBufferData>(context, size, K_SLOT_CAMERAS, K_FALSE) {}

        public:
            void SetCamera(PerspectiveCameraComponent* camera);
            void SetCamera(OrthoCameraComponent* camera);
            void SetPerspectiveProjection(u32 index, const math::PerspectiveMatrix& projection);
            void SetOrthoProjection(u32 index, const math::OrthoMatrix& projection);
            void SetView(CameraComponent* camera);
            void SetPosition(CameraComponent* camera);

        };

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

        class ENGINE_API OrthoCameraController : public Camera {

        public:
            OrthoCameraComponent* Component = nullptr;

        public:
            OrthoCameraController(CameraBuffer* cameraBuffer, OrthoCameraComponent* component, Time* time);

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
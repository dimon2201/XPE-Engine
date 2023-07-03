#pragma once

#include <core/user_input.hpp>

namespace xpe {

    namespace core {

        struct ENGINE_API cCameraComponent : public cComponent
        {
            cCameraComponent(const std::string& usid) : cComponent(usid)
            {}

            glm::vec3 Position = { 0, 0, 10 };
            glm::vec3 Front = { 0, 0, -1 };
            glm::vec3 Up = { 0, 1, 0 };

            float Pitch = 0;
            float Yaw = 0;
            float Roll = 0;
        };

        struct ENGINE_API cPerspectiveCameraComponent : public cCameraComponent
        {
            cPerspectiveCameraComponent(const std::string& usid) : cCameraComponent(usid)
            {}

            math::PerspectiveMatrix Projection;
        };

        struct ENGINE_API cOrthoCameraComponent : public cCameraComponent
        {
            cOrthoCameraComponent(const std::string& usid) : cCameraComponent(usid)
            {}

            math::OrthoMatrix Projection;
        };

        struct ENGINE_API CameraBufferData final {
            glm::mat4 Projection;
            glm::mat4 View;
            glm::vec3 Position;
            float padding_0 = 0;
        };

        class ENGINE_API CameraBuffer : public xBuffer {

        public:
            void Init(RenderingContext_Interface* context);

            void Free();

            void Flush();

            void SetCamera(cPerspectiveCameraComponent* camera);
            void SetCamera(cOrthoCameraComponent* camera);
            void SetPerspectiveProjection(const math::PerspectiveMatrix& projection);
            void SetOrthoProjection(const math::OrthoMatrix& projection);
            void SetView(cCameraComponent* camera);
            void SetPosition(cCameraComponent* camera);

        private:
            RenderingContext_Interface* m_Context = nullptr;
            CameraBufferData m_Data;
        };

        class cCameraController : public Object,
                public CursorEventListener,
                public WindowEventListener,
                public ScrollEventListener
        {

        public:
            eKey KeyMoveForward = eKey::W;
            eKey KeyMoveLeft = eKey::A;
            eKey KeyMoveBackward = eKey::S;
            eKey KeyMoveRight = eKey::D;

            float MoveSpeed = 0.01f;
            float ZoomSpeed = 3.0f;

            float HorizontalSensitivity = 1.0f;
            float VerticalSensitivity = 1.0f;

        public:
            cCameraController(cUserInputManager* userInput, CameraBuffer* cameraBuffer, Time* time);
            virtual ~cCameraController() override;

            void EnableLook();
            void EnableZoom();

            void DisableLook();
            void DisableZoom();

            virtual void Move() = 0;
            virtual void ZoomIn() = 0;
            virtual void ZoomOut() = 0;
            virtual void Look(const double x, const double y) = 0;

            void CursorMoved(const double x, const double y) override;

        protected:
            cUserInputManager* m_UserInput = nullptr;
            CameraBuffer* m_CameraBuffer = nullptr;
            Time* m_Time = nullptr;
        };

        class ENGINE_API cPerspectiveCameraController : public cCameraController {

        public:
            cPerspectiveCameraComponent* Camera = nullptr;
            float MaxFovDegree = 45.0f;
            float MinFovDegree = 1.0f;

        public:
            cPerspectiveCameraController(cUserInputManager* userInput, CameraBuffer* cameraBuffer, cPerspectiveCameraComponent* camera, Time* time)
            : cCameraController(userInput, cameraBuffer, time), Camera(camera), MaxFovDegree(camera->Projection.FovDegree) {
                m_CameraBuffer->SetCamera(camera);
            }

            void Move() override;

            void ZoomIn() override;

            void ZoomOut() override;

            void Look(const double x, const double y) override;

            void ScrollChanged(const double x, const double y) override;
        };

        class ENGINE_API cOrthoCameraController : public cCameraController {

        public:
            cOrthoCameraComponent* Camera = nullptr;

        public:
            cOrthoCameraController(cUserInputManager* userInput, CameraBuffer* cameraBuffer, cOrthoCameraComponent* camera, Time* time)
            : cCameraController(userInput, cameraBuffer, time), Camera(camera) {
                cameraBuffer->SetCamera(camera);
            }

            void Move() override;

            void ZoomIn() override;

            void ZoomOut() override;

            void Look(const double x, const double y) override;

        };

    }

}
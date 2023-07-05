#include <rendering/context.hpp>
#include <controls/camera.h>

namespace xpe {

    namespace control {

        void CameraBuffer::Init(Context *context) {
            m_Context = context;
            m_Context->CreateBuffer(*this, eBufferType::CONSTANT, sizeof(CameraBufferData), K_FALSE);
        }

        void CameraBuffer::Free() {
            m_Context->FreeBuffer(*this);
        }

        void CameraBuffer::Flush() {
            m_Context->WriteBuffer(*this, &m_Data, sizeof(m_Data));
        }

        void CameraBuffer::SetCamera(cPerspectiveCameraComponent *camera) {
            SetPerspectiveProjection(camera->Projection);
            SetView(camera);
            SetPosition(camera);
        }

        void CameraBuffer::SetCamera(cOrthoCameraComponent *camera) {
            SetOrthoProjection(camera->Projection);
            SetView(camera);
            SetPosition(camera);
        }

        void CameraBuffer::SetPerspectiveProjection(const math::PerspectiveMatrix &projection) {
            m_Data.Projection = math::PerspectiveMatrixUpdate(projection);
        }

        void CameraBuffer::SetOrthoProjection(const math::OrthoMatrix &projection) {
            m_Data.Projection = math::OrthoMatrixUpdate(projection);
        }

        void CameraBuffer::SetView(cCameraComponent *camera) {
            math::ViewMatrix view;
            view.Eye = camera->Position;
            view.Up = camera->Up;
            view.Center = camera->Position + camera->Front;
            m_Data.View = math::ViewMatrixUpdate(view);
        }

        void CameraBuffer::SetPosition(cCameraComponent *camera) {
            m_Data.Position = camera->Position;
        }

        cCameraController::cCameraController(cUserInputManager* userInput, CameraBuffer* cameraBuffer, Time* time)
        : m_UserInput(userInput), m_CameraBuffer(cameraBuffer), m_Time(time) {
            EnableLook();
            EnableZoom();
            m_UserInput->AddWindowEventListener(this, 2);
        }

        cCameraController::~cCameraController() {
            DisableLook();
            DisableZoom();
            m_UserInput->RemoveWindowEventListener(this);
        }

        void cCameraController::EnableLook() {
            m_UserInput->AddCursorEventListener(this, 2);
        }

        void cCameraController::EnableZoom() {
            m_UserInput->AddScrollEventListener(this, 2);
        }

        void cCameraController::DisableLook() {
            m_UserInput->RemoveCursorEventListener(this);
        }

        void cCameraController::DisableZoom() {
            m_UserInput->RemoveScrollEventListener(this);
        }

        void cCameraController::CursorMoved(const double x, const double y) {
            Look(x, y);
        }

        void cPerspectiveCameraController::Move() {
            float dt = m_Time->Millis();
            float distance = MoveSpeed / dt;
            auto& position = Camera->Position;
            auto& front = Camera->Front;
            auto& up = Camera->Up;

            if (m_UserInput->isKeyPressed(KeyMoveForward)) {
                position += distance * front;
            }

            else if (m_UserInput->isKeyPressed(KeyMoveLeft)) {
                position += glm::normalize(glm::cross(front, up)) * distance;
            }

            else if (m_UserInput->isKeyPressed(KeyMoveBackward)) {
                position -= distance * front;
            }

            else if (m_UserInput->isKeyPressed(KeyMoveRight)) {
                position -= glm::normalize(glm::cross(front, up)) * distance;
            }

            else
                return;

            m_CameraBuffer->SetView(Camera);
            m_CameraBuffer->SetPosition(Camera);
            m_CameraBuffer->Flush();
        }

        void cPerspectiveCameraController::ZoomIn() {
            float dt = m_Time->Millis();
            auto& fov = Camera->Projection.FovDegree;

            fov -= (float) ZoomSpeed / dt;
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            m_CameraBuffer->SetPerspectiveProjection(Camera->Projection);
            m_CameraBuffer->Flush();
        }

        void cPerspectiveCameraController::ZoomOut() {
            float dt = m_Time->Millis();
            auto& fov = Camera->Projection.FovDegree;

            fov += (float) ZoomSpeed / dt;
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            m_CameraBuffer->SetPerspectiveProjection(Camera->Projection);
            m_CameraBuffer->Flush();
        }

        void cPerspectiveCameraController::ScrollChanged(const double x, const double y) {
            float dt = m_Time->Millis();
            auto& fov = Camera->Projection.FovDegree;

            fov -= (float) y * ZoomSpeed / dt;
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            LogInfo("Zoomed FOV {}", fov);

            m_CameraBuffer->SetPerspectiveProjection(Camera->Projection);
            m_CameraBuffer->Flush();
        }

        void cPerspectiveCameraController::Look(const double x, const double y) {
            m_UserInput->CaptureCursor();

            auto& cursorDelta = m_UserInput->GetMouseCursor().Delta;
            float dt = m_Time->Millis();
            auto& front = Camera->Front;
            auto& up = Camera->Up;

            if (cursorDelta.x != 0 || cursorDelta.y != 0) {
                float pitchDt = cursorDelta.y * VerticalSensitivity * 0.001f / dt;
                float yawDt = cursorDelta.x * HorizontalSensitivity * 0.001f / dt;
                glm::vec3 right = glm::cross(front, up);
                glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDt, right), glm::angleAxis(yawDt, up)));
                front = glm::rotate(q, front);
                m_CameraBuffer->SetView(Camera);
                m_CameraBuffer->Flush();
            }
        }

        void cPerspectiveCameraController::WindowFrameResized(int width, int height) {
            Camera->Projection.AspectRatio = static_cast<float>(width) / static_cast<float>(height);
            m_CameraBuffer->SetPerspectiveProjection(Camera->Projection);
            m_CameraBuffer->Flush();
        }

        void cOrthoCameraController::Move() {
            // todo ortho camera move function
        }

        void cOrthoCameraController::ZoomIn() {
            // todo ortho camera zoom in function
        }

        void cOrthoCameraController::ZoomOut() {
            // todo ortho camera zoom out function
        }

        void cOrthoCameraController::Look(const double x, const double y) {
            // todo ortho camera look function
        }

    }

}
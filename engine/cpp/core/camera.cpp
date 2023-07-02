#include <core/camera.h>

namespace xpe {

    namespace core {

        void CameraBuffer::Init(RenderingContext_Interface *context) {
            m_Context = context;
            m_Context->CreateBuffer(*this, xType::CONSTANT, sizeof(CameraBufferData), K_FALSE);
        }

        void CameraBuffer::Free() {
            m_Context->FreeBuffer(*this);
        }

        void CameraBuffer::Flush() {
            if (m_Updated) {
                m_Context->WriteBuffer(*this, &m_Data, sizeof(m_Data));
                m_Updated = false;
            }
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
            m_Updated = true;
        }

        void CameraBuffer::SetOrthoProjection(const math::OrthoMatrix &projection) {
            m_Data.Projection = math::OrthoMatrixUpdate(projection);
            m_Updated = true;
        }

        void CameraBuffer::SetView(cCameraComponent *camera) {
            math::ViewMatrix view;
            view.Eye = camera->Position;
            view.Up = camera->Up;
            view.Center = camera->Position + camera->Front;
            m_Data.View = math::ViewMatrixUpdate(view);
            m_Updated = true;
        }

        void CameraBuffer::SetPosition(cCameraComponent *camera) {
            m_Data.Position = camera->Position;
            m_Updated = true;
        }

        cCameraController::cCameraController(cUserInputManager* userInput, CameraBuffer* cameraBuffer, float *dt)
        : m_UserInput(userInput), m_CameraBuffer(cameraBuffer), m_Dt(dt) {
            EnableMove();
            EnableLook();
            EnableZoom();
        }

        cCameraController::~cCameraController() {
        }

        void cCameraController::EnableMove() {
            m_EnableMove = true;
        }

        void cCameraController::EnableLook() {
            m_UserInput->AddCursorEventListener(this, 2);
        }

        void cCameraController::EnableZoom() {
            m_UserInput->AddScrollEventListener(this, 2);
        }

        void cCameraController::DisableMove() {
            m_EnableMove = false;
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
            if (!m_EnableMove)
                return;

            float dt = *m_Dt;
            float distance = MoveSpeed / dt;
            auto& position = Camera->Position;
            auto& front = Camera->Front;
            auto& up = Camera->Up;

            if (m_UserInput->isKeyPressed(KeyMoveForward)) {
                position += distance * front;
            }

            else if (m_UserInput->isKeyPressed(KeyMoveLeft)) {
                position -= glm::normalize(glm::cross(front, up)) * distance;
            }

            else if (m_UserInput->isKeyPressed(KeyMoveBackward)) {
                position -= distance * front;
            }

            else if (m_UserInput->isKeyPressed(KeyMoveRight)) {
                position += glm::normalize(glm::cross(front, up)) * distance;
            }

            else
                return;

            m_CameraBuffer->SetView(Camera);
            m_CameraBuffer->SetPosition(Camera);
        }

        void cPerspectiveCameraController::ZoomIn() {
            float dt = *m_Dt;
            auto& fov = Camera->Projection.FovDegree;

            fov -= (float) ZoomSpeed / dt;
            math::clamp(fov, 1.0f, MaxFovDegree);

            m_CameraBuffer->SetPerspectiveProjection(Camera->Projection);
        }

        void cPerspectiveCameraController::ZoomOut() {
            float dt = *m_Dt;
            auto& fov = Camera->Projection.FovDegree;

            fov -= (float) ZoomSpeed / dt;
            math::clamp(fov, 1.0f, MaxFovDegree);

            m_CameraBuffer->SetPerspectiveProjection(Camera->Projection);
        }

        void cPerspectiveCameraController::ScrollChanged(const double x, const double y) {
            float dt = *m_Dt;
            auto& fov = Camera->Projection.FovDegree;

            fov -= (float) y * ZoomSpeed / dt;
            math::clamp(fov, 1.0f, MaxFovDegree);

            m_CameraBuffer->SetPerspectiveProjection(Camera->Projection);
        }

        void cPerspectiveCameraController::Look(const double x, const double y) {
            m_UserInput->CaptureCursor();

            auto& cursorDelta = m_UserInput->GetMouseCursor().Delta;
            float& dt = *m_Dt;
            auto& front = Camera->Front;
            auto& up = Camera->Up;

            if (cursorDelta.x != 0 || cursorDelta.y != 0) {
                float pitchDt = cursorDelta.y * VerticalSensitivity * 0.001f / dt;
                float yawDt = cursorDelta.x * HorizontalSensitivity * 0.001f / dt;
                glm::vec3 right = glm::cross(front, up);
                glm::quat q = glm::normalize(glm::cross(glm::angleAxis(pitchDt, right), glm::angleAxis(-yawDt, up)));
                front = glm::rotate(q, front);
                m_CameraBuffer->SetView(Camera);
            }
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
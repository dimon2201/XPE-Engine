#include <controls/camera.h>

namespace xpe {

    namespace control {

        glm::vec3 cCameraComponent::GetUpDirection() const {
            return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        glm::vec3 cCameraComponent::GetRightDirection() const {
            return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
        }

        glm::vec3 cCameraComponent::GetForwardDirection() const {
            return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
        }

        glm::vec3 cCameraComponent::CalculatePosition() const {
            return FocalPoint - GetForwardDirection() * Distance;
        }

        glm::quat cCameraComponent::GetOrientation() const {
            return glm::vec3(-Pitch, -Yaw, -Roll);
        }

        void CameraBuffer::SetCamera(cPerspectiveCameraComponent *camera) {
            SetPerspectiveProjection(camera->Index, camera->Projection);
            SetView(camera);
            SetPosition(camera);
        }

        void CameraBuffer::SetCamera(cOrthoCameraComponent *camera) {
            SetOrthoProjection(camera->Index, camera->Projection);
            SetView(camera);
            SetPosition(camera);
        }

        void CameraBuffer::SetPerspectiveProjection(u32 index, const math::PerspectiveMatrix &projection) {
            GetItem(index)->Projection = math::PerspectiveMatrixUpdate(projection);
        }

        void CameraBuffer::SetOrthoProjection(u32 index, const math::OrthoMatrix &projection) {
            GetItem(index)->Projection = math::OrthoMatrixUpdate(projection);
        }

        void CameraBuffer::SetView(cCameraComponent *camera) {
            math::ViewMatrix view;
            view.Eye = camera->Position;
            view.Up = camera->Up;
            view.Center = camera->Position + camera->Front;
            GetItem(camera->Index)->View = math::ViewMatrixUpdate(view);
        }

        void CameraBuffer::SetPosition(cCameraComponent *camera) {
            GetItem(camera->Index)->Position = camera->Position;
        }

        cCameraController::cCameraController(CameraBuffer* cameraBuffer, Time* time)
        : m_CameraBuffer(cameraBuffer), m_Dt(time) {
            EnableLook();
            EnableZoom();
            Input::AddWindowEventListener(this, 2);
        }

        cCameraController::~cCameraController() {
            DisableLook();
            DisableZoom();
            Input::RemoveWindowEventListener(this);
        }

        void cCameraController::EnableLook() {
            Input::AddCursorEventListener(this, 2);
        }

        void cCameraController::EnableZoom() {
            Input::AddScrollEventListener(this, 2);
        }

        void cCameraController::DisableLook() {
            Input::RemoveCursorEventListener(this);
        }

        void cCameraController::DisableZoom() {
            Input::RemoveScrollEventListener(this);
        }

        void cCameraController::CursorMoved(const double x, const double y) {
            Look(x, y);
        }

        void cPerspectiveCameraController::Move() {
            float dt = m_Dt->Millis();
            float distance = MoveSpeed * 0.1f / dt;
            auto& position = Camera->Position;
            auto front = glm::normalize(Camera->Front);
            auto up = glm::normalize(Camera->Up);
            auto right = glm::cross(front, up);

            if (Input::KeyPressed(KeyMoveForward)) {
                position += front * distance;
            }

            else if (Input::KeyPressed(KeyMoveLeft)) {
                position += right * distance;
            }

            else if (Input::KeyPressed(KeyMoveBackward)) {
                position -= front * distance;
            }

            else if (Input::KeyPressed(KeyMoveRight)) {
                position -= right * distance;
            }

            else
                return;

            m_CameraBuffer->SetView(Camera);
            m_CameraBuffer->SetPosition(Camera);
            m_CameraBuffer->Flush();
        }

        void cPerspectiveCameraController::ZoomIn() {
            float dt = m_Dt->Millis();
            auto& fov = Camera->Projection.FovDegree;

            fov -= (float) ZoomSpeed / dt;
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            m_CameraBuffer->SetPerspectiveProjection(Camera->Index, Camera->Projection);
            m_CameraBuffer->Flush();
        }

        void cPerspectiveCameraController::ZoomOut() {
            float dt = m_Dt->Millis();
            auto& fov = Camera->Projection.FovDegree;

            fov += (float) ZoomSpeed / dt;
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            m_CameraBuffer->SetPerspectiveProjection(Camera->Index, Camera->Projection);
            m_CameraBuffer->Flush();
        }

        void cPerspectiveCameraController::ScrollChanged(const double x, const double y) {
            float dt = m_Dt->Millis();
            auto& fov = Camera->Projection.FovDegree;

            fov -= (float) y * ZoomSpeed / dt;
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            m_CameraBuffer->SetPerspectiveProjection(Camera->Index, Camera->Projection);
            m_CameraBuffer->Flush();
        }

        void cPerspectiveCameraController::Look(const double x, const double y) {
            Input::CaptureCursor(x, y);

            auto& cursorDelta = Input::GetMouseCursor().Delta;
            float dt = m_Dt->Millis();
            auto& front = Camera->Front;
            auto& up = Camera->Up;

            if (cursorDelta.x != 0 || cursorDelta.y != 0) {
                float pitchDt = cursorDelta.y * VerticalSensitivity * 0.1f / dt;
                float yawDt = cursorDelta.x * HorizontalSensitivity * 0.1f / dt;
                glm::vec3 right = glm::cross(front, up);
                glm::quat quatRight = glm::normalize(glm::angleAxis(-pitchDt, right));
                glm::quat quatUp = glm::normalize(glm::angleAxis(yawDt, up));
                glm::quat quatFront = glm::cross(quatRight, quatUp);
                up = glm::rotate(quatUp, up);
                front = glm::rotate(quatFront, front);
                m_CameraBuffer->SetView(Camera);
                m_CameraBuffer->Flush();
                LogGLM("Look Up vector", up);
            }
        }

        void cPerspectiveCameraController::WindowFrameResized(int width, int height) {
            Camera->Projection.AspectRatio = static_cast<float>(width) / static_cast<float>(height);
            m_CameraBuffer->SetPerspectiveProjection(Camera->Index, Camera->Projection);
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
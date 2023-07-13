#include <controls/camera.h>

namespace xpe {

    namespace control {

        glm::vec3 CameraComponent::GetUpDirection() const {
            return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        glm::vec3 CameraComponent::GetRightDirection() const {
            return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
        }

        glm::vec3 CameraComponent::GetForwardDirection() const {
            return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
        }

        glm::vec3 CameraComponent::CalculatePosition() const {
            return FocalPoint - GetForwardDirection() * Distance;
        }

        glm::quat CameraComponent::GetOrientation() const {
            return glm::vec3(-Pitch, -Yaw, -Roll);
        }

        void CameraBuffer::SetCamera(PerspectiveCameraComponent *camera) {
            SetPerspectiveProjection(camera->Index, camera->Projection);
            SetView(camera);
            SetPosition(camera);
        }

        void CameraBuffer::SetCamera(OrthoCameraComponent *camera) {
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

        void CameraBuffer::SetView(CameraComponent *camera) {
            math::ViewMatrix view;
            view.Eye = camera->Position;
            view.Up = camera->Up;
            view.Center = camera->Position + camera->Front;
            GetItem(camera->Index)->View = math::ViewMatrixUpdate(view);
        }

        void CameraBuffer::SetPosition(CameraComponent *camera) {
            GetItem(camera->Index)->Position = camera->Position;
        }

        Camera::Camera(CameraBuffer* cameraBuffer, Time* time)
        : m_CameraBuffer(cameraBuffer), m_Dt(time) {}

        PerspectiveCamera::PerspectiveCamera(
                CameraBuffer *cameraBuffer,
                PerspectiveCameraComponent *component,
                Time *time
        ) : Camera(cameraBuffer, time),
            Component(component),
            MaxFovDegree(component->Projection.FovDegree)
        {
            m_CameraBuffer->SetCamera(component);
            m_CameraBuffer->Flush();
            Input::WindowFrameResizedEvents.AddEvent(this, core::OnWindowFrameResized<PerspectiveCamera>, 2);
            Input::ScrollChangedEvents.AddEvent(this, core::OnScrollChanged<PerspectiveCamera>, 2);
            Input::CursorMovedEvents.AddEvent(this, core::OnCursorMoved<PerspectiveCamera>, 2);
        }

        void PerspectiveCamera::Move() {
            float dt = m_Dt->Millis();
            float distance = MoveSpeed * 0.1f / dt;
            auto& position = Component->Position;
            auto front = glm::normalize(Component->Front);
            auto up = glm::normalize(Component->Up);
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

            m_CameraBuffer->SetView(Component);
            m_CameraBuffer->SetPosition(Component);
            m_CameraBuffer->Flush();
        }

        void PerspectiveCamera::ZoomIn() {
            float dt = m_Dt->Millis();
            auto& fov = Component->Projection.FovDegree;

            fov -= (float) ZoomSpeed / dt;
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            m_CameraBuffer->SetPerspectiveProjection(Component->Index, Component->Projection);
            m_CameraBuffer->Flush();
        }

        void PerspectiveCamera::ZoomOut() {
            float dt = m_Dt->Millis();
            auto& fov = Component->Projection.FovDegree;

            fov += (float) ZoomSpeed / dt;
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            m_CameraBuffer->SetPerspectiveProjection(Component->Index, Component->Projection);
            m_CameraBuffer->Flush();
        }

        void PerspectiveCamera::ScrollChanged(const double x, const double y) {
            float dt = m_Dt->Millis();
            auto& fov = Component->Projection.FovDegree;

            fov -= (float) y * ZoomSpeed / dt;
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            m_CameraBuffer->SetPerspectiveProjection(Component->Index, Component->Projection);
            m_CameraBuffer->Flush();
        }

        void PerspectiveCamera::Look(const double x, const double y) {
            Input::CaptureCursor(x, y);

            auto& cursorDelta = Input::GetMouseCursor().Delta;
            float dt = m_Dt->Millis();
            auto& front = Component->Front;
            auto& up = Component->Up;

            if (cursorDelta.x != 0 || cursorDelta.y != 0) {
                float pitchDt = cursorDelta.y * VerticalSensitivity * 0.1f / dt;
                float yawDt = cursorDelta.x * HorizontalSensitivity * 0.1f / dt;
                glm::vec3 right = glm::cross(front, up);
                glm::quat quatRight = glm::normalize(glm::angleAxis(-pitchDt, right));
                glm::quat quatUp = glm::normalize(glm::angleAxis(yawDt, up));
                glm::quat quatFront = glm::cross(quatRight, quatUp);
                up = glm::rotate(quatUp, up);
                front = glm::rotate(quatFront, front);
                m_CameraBuffer->SetView(Component);
                m_CameraBuffer->Flush();
//                LogGLM("Look Up vector", up);
            }
        }

        void PerspectiveCamera::WindowFrameResized(int width, int height) {
            Component->Projection.AspectRatio = static_cast<float>(width) / static_cast<float>(height);
            m_CameraBuffer->SetPerspectiveProjection(Component->Index, Component->Projection);
            m_CameraBuffer->Flush();
        }

        void PerspectiveCamera::CursorMoved(const double x, const double y) {
            Look(x, y);
        }

        OrthoCameraController::OrthoCameraController(
                CameraBuffer *cameraBuffer,
                OrthoCameraComponent *component,
                Time *time
        ) : Camera(cameraBuffer, time), Component(component) {
            m_CameraBuffer->SetCamera(component);
            m_CameraBuffer->Flush();
            Input::WindowFrameResizedEvents.AddEvent(this, core::OnWindowFrameResized<OrthoCameraController>, 2);
            Input::ScrollChangedEvents.AddEvent(this, core::OnScrollChanged<OrthoCameraController>, 2);
            Input::CursorMovedEvents.AddEvent(this, core::OnCursorMoved<OrthoCameraController>, 2);
        }

        void OrthoCameraController::Move() {
            // todo ortho camera move function
        }

        void OrthoCameraController::ZoomIn() {
            // todo ortho camera zoom in function
        }

        void OrthoCameraController::ZoomOut() {
            // todo ortho camera zoom out function
        }

        void OrthoCameraController::Look(const double x, const double y) {
            // todo ortho camera look function
        }

        void OrthoCameraController::WindowFrameResized(int w, int h) {
            Component->Projection.Left = w;
            Component->Projection.Top = h;
            m_CameraBuffer->SetOrthoProjection(Component->Index, Component->Projection);
            m_CameraBuffer->Flush();
        }

        void OrthoCameraController::ScrollChanged(const double x, const double y) {

        }

        void OrthoCameraController::CursorMoved(const double x, const double y) {
            Look(x, y);
        }

    }

}
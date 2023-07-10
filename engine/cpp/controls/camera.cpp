#include <rendering/context.hpp>
#include <controls/camera.h>

namespace xpe {

    namespace control {

        void CameraBuffer::Init(Context *context) {
            m_Context = context;
            Type = eBufferType::CONSTANT;
            ByteSize = sizeof(CameraBufferData);
            Slot = 0;
            m_Context->CreateBuffer(*this, K_FALSE);
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

        cCameraController::cCameraController(Context* context, Time* time)
        : m_Time(time) {
            m_CameraBuffer.Init(context);
            EnableLook();
            EnableZoom();
            Input::AddWindowEventListener(this, 2);
        }

        cCameraController::~cCameraController() {
            m_CameraBuffer.Free();
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
            float dt = m_Time->Millis();
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

            m_CameraBuffer.SetView(Camera);
            m_CameraBuffer.SetPosition(Camera);
            m_CameraBuffer.Flush();
        }

        void cPerspectiveCameraController::ZoomIn() {
            float dt = m_Time->Millis();
            auto& fov = Camera->Projection.FovDegree;

            fov -= (float) ZoomSpeed / dt;
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            m_CameraBuffer.SetPerspectiveProjection(Camera->Projection);
            m_CameraBuffer.Flush();
        }

        void cPerspectiveCameraController::ZoomOut() {
            float dt = m_Time->Millis();
            auto& fov = Camera->Projection.FovDegree;

            fov += (float) ZoomSpeed / dt;
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            m_CameraBuffer.SetPerspectiveProjection(Camera->Projection);
            m_CameraBuffer.Flush();
        }

        void cPerspectiveCameraController::ScrollChanged(const double x, const double y) {
            float dt = m_Time->Millis();
            auto& fov = Camera->Projection.FovDegree;

            fov -= (float) y * ZoomSpeed / dt;
            math::clamp(fov, MinFovDegree, MaxFovDegree);

            m_CameraBuffer.SetPerspectiveProjection(Camera->Projection);
            m_CameraBuffer.Flush();
        }

        void cPerspectiveCameraController::Look(const double x, const double y) {
            Input::CaptureCursor();

            auto& cursorDelta = Input::GetMouseCursor().Delta;
            float dt = m_Time->Millis();
            auto& front = Camera->Front;
            auto& up = Camera->Up;

            if (cursorDelta.x != 0 || cursorDelta.y != 0) {
                float pitchDt = cursorDelta.y * VerticalSensitivity * 0.1f / dt;
                float yawDt = cursorDelta.x * HorizontalSensitivity * 0.1f / dt;
                glm::vec3 right = glm::cross(front, up);
                glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDt, right), glm::angleAxis(yawDt, up)));
                front = glm::rotate(q, front);
                m_CameraBuffer.SetView(Camera);
                m_CameraBuffer.Flush();
            }
        }

        void cPerspectiveCameraController::WindowFrameResized(int width, int height) {
            Camera->Projection.AspectRatio = static_cast<float>(width) / static_cast<float>(height);
            m_CameraBuffer.SetPerspectiveProjection(Camera->Projection);
            m_CameraBuffer.Flush();
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
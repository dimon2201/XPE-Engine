#include <rendering/buffers/camera_buffer.h>

namespace xpe {

    namespace render {

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

    }

}
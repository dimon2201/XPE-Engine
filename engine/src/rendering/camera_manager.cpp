#include <rendering/camera_manager.hpp>
#include <rendering/bindings.hpp>

namespace xpe {

    namespace render {

        cCamera* cCameraManager::s_Camera = nullptr;

        void cCameraManager::Init()
        {
            Buffers::Camera = new sCameraBuffer();
        }

        void cCameraManager::Free()
        {
            delete Buffers::Camera;
            delete s_Camera;
        }

        glm::mat4 cCameraManager::GetViewProjection()
        {
            return Buffers::Camera->Item.Projection * Buffers::Camera->Item.View;
        }

        void cCameraManager::Flush()
        {
            Buffers::Camera->Flush();
        }

        void cCameraManager::Bind()
        {
            context::VSBindBuffer(*Buffers::Camera);
        }

        void cCameraManager::Unbind()
        {
            context::VSUnbindBuffer(*Buffers::Camera);
        }

        void cCameraManager::SetExposure(float exposure)
        {
            Buffers::Camera->Item.Exposure = exposure;
            Buffers::Camera->Flush();
        }

        void cCameraManager::SetGamma(float gamma)
        {
            Buffers::Camera->Item.Gamma = gamma;
            Buffers::Camera->Flush();
        }

        float cCameraManager::GetExposure()
        {
            return Buffers::Camera->Item.Exposure;
        }

        float cCameraManager::GetGamma()
        {
            return Buffers::Camera->Item.Gamma;
        }

        void cCameraManager::SetViewport(const sViewport &viewport)
        {
            Buffers::Camera->Item.Viewport = viewport;
            Buffers::Camera->Flush();
        }

        sViewport* cCameraManager::GetViewport()
        {
            return &Buffers::Camera->Item.Viewport;
        }

        cCamera* cCameraManager::GetCamera()
        {
            return s_Camera;
        }

        void cCameraManager::SetCamera(cCamera* camera)
        {
            s_Camera = camera;
        }

    }

}
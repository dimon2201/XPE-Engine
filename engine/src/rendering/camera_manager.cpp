#include <rendering/camera_manager.hpp>
#include <rendering/bindings.hpp>

namespace xpe {

    namespace render {

        cCamera* cCameraManager::s_Camera = nullptr;

        void cCameraManager::Init()
        {
            Buffers::Camera = new cCameraBuffer();
        }

        void cCameraManager::Free()
        {
            delete Buffers::Camera;
            delete s_Camera;
        }

        glm::mat4 cCameraManager::GetViewProjection()
        {
            return Buffers::Camera->GetItem().Projection * Buffers::Camera->GetItem().View;
        }

        void cCameraManager::Flush()
        {
            Buffers::Camera->Flush();
        }

        void cCameraManager::Bind()
        {
            context::VSBindBuffer(cResource::eViewType::SRV, cBuffer::eType::STRUCTURED, Buffers::Camera->GetSlot(), Buffers::Camera->GetInstance(), Buffers::Camera->GetSRVInstance());
        }

        void cCameraManager::Unbind()
        {
            context::VSUnbindBuffer(*Buffers::Camera);
        }

        void cCameraManager::SetExposure(float exposure)
        {
            Buffers::Camera->GetItem().Exposure = exposure;
            Buffers::Camera->Flush();
        }

        void cCameraManager::SetGamma(float gamma)
        {
            Buffers::Camera->GetItem().Gamma = gamma;
            Buffers::Camera->Flush();
        }

        float cCameraManager::GetExposure()
        {
            return Buffers::Camera->GetItem().Exposure;
        }

        float cCameraManager::GetGamma()
        {
            return Buffers::Camera->GetItem().Gamma;
        }

        void cCameraManager::SetViewport(const sViewport &viewport)
        {
            Buffers::Camera->GetItem().Viewport = viewport;
            Buffers::Camera->Flush();
        }

        sViewport* cCameraManager::GetViewport()
        {
            return &Buffers::Camera->GetItem().Viewport;
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
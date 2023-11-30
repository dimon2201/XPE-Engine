#include <rendering/camera_manager.h>

namespace xpe {

    namespace render {

        sCameraBuffer* cCameraManager::s_Buffer = nullptr;
        cCamera* cCameraManager::s_Camera = nullptr;

        void cCameraManager::Init()
        {
            s_Buffer = new sCameraBuffer();
        }

        void cCameraManager::Free()
        {
            delete s_Buffer;
            delete s_Camera;
        }

        sCameraBuffer* cCameraManager::GetBuffer()
        {
            return s_Buffer;
        }

        glm::mat4 cCameraManager::GetViewProjection()
        {
            return s_Buffer->Item.Projection * s_Buffer->Item.View;
        }

        void cCameraManager::Flush()
        {
            s_Buffer->Flush();
        }

        void cCameraManager::Bind()
        {
            context::VSBindBuffer(*s_Buffer);
        }

        void cCameraManager::Unbind()
        {
            context::VSUnbindBuffer(*s_Buffer);
        }

        void cCameraManager::SetExposure(float exposure)
        {
            s_Buffer->Item.Exposure = exposure;
            s_Buffer->Flush();
        }

        void cCameraManager::SetGamma(float gamma)
        {
            s_Buffer->Item.Gamma = gamma;
            s_Buffer->Flush();
        }

        float cCameraManager::GetExposure()
        {
            return s_Buffer->Item.Exposure;
        }

        float cCameraManager::GetGamma()
        {
            return s_Buffer->Item.Gamma;
        }

        void cCameraManager::SetViewport(const sViewport &viewport)
        {
            s_Buffer->Item.Viewport = viewport;
            s_Buffer->Flush();
        }

        sViewport* cCameraManager::GetViewport()
        {
            return &s_Buffer->Item.Viewport;
        }

        cPerspectiveCamera* cCameraManager::AddPerspectiveCamera(int width, int height)
        {
            return new cPerspectiveCamera(width, height, s_Buffer);
        }

        cOrthoCamera* cCameraManager::AddOrthoCamera(int width, int height)
        {
            return new cOrthoCamera(width, height, s_Buffer);
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
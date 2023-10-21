#include <rendering/render_manager.h>
#include <rendering/monitor.h>
#include <rendering/buffers/camera_buffer.h>
#include <rendering/buffers/light_buffers.h>
#include <rendering/shadow/shadow.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        CameraBuffer* RenderManager::m_CameraBuffer = nullptr;
        DirectLightBuffer* RenderManager::m_DirectLightBuffer = nullptr;
        PointLightBuffer* RenderManager::m_PointLightBuffer = nullptr;
        SpotLightBuffer* RenderManager::m_SpotLightBuffer = nullptr;
        vector<RenderPass*>* RenderManager::m_OpaqueRenderPasses = nullptr;
        vector<RenderPass*>* RenderManager::m_TransparentRenderPasses = nullptr;
        vector<RenderPass*>* RenderManager::m_GUIRenderPasses = nullptr;
        Texture* RenderManager::m_SharedDepthTexture = nullptr;
        RenderTarget* RenderManager::m_OpaqueRenderTarget = nullptr;
        RenderTarget* RenderManager::m_TransparentRenderTarget = nullptr;

        void RenderManager::Init(Viewport* viewport, core::Boolean useMSAA, core::usize msaaSampleCount)
        {
            m_CameraBuffer = new render::CameraBuffer();

            m_DirectLightBuffer = new render::DirectLightBuffer();
            m_DirectLightBuffer->Reserve(1000);

            m_PointLightBuffer = new render::PointLightBuffer();
            m_PointLightBuffer->Reserve(1000);

            m_SpotLightBuffer = new render::SpotLightBuffer();
            m_SpotLightBuffer->Reserve(1000);

            m_OpaqueRenderPasses = new vector<RenderPass*>();
            m_TransparentRenderPasses = new vector<RenderPass*>();
            m_GUIRenderPasses = new vector<RenderPass*>();

            // Shared depth texture for opaque and transparent render targets
            m_SharedDepthTexture = new Texture();
            m_SharedDepthTexture->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            m_SharedDepthTexture->Width = viewport->Width;
            m_SharedDepthTexture->Height = viewport->Height;
            m_SharedDepthTexture->Format = eTextureFormat::R32_TYPELESS;
            m_SharedDepthTexture->InitializeData = false;
            m_SharedDepthTexture->EnableRenderTarget = true;
            m_SharedDepthTexture->SampleCount = useMSAA == core::K_TRUE ? msaaSampleCount : 1;
            m_SharedDepthTexture->Init();

            // Opaque render target
            Texture* mainColor = new Texture();
            mainColor->Width = viewport->Width;
            mainColor->Height = viewport->Height;
            mainColor->Format = eTextureFormat::RGBA8;
            mainColor->InitializeData = false;
            mainColor->EnableRenderTarget = true;
            mainColor->SampleCount = useMSAA == core::K_TRUE ? msaaSampleCount : 1;
            mainColor->Init();

            Texture* mainPosition = new Texture();
            mainPosition->Width = viewport->Width;
            mainPosition->Height = viewport->Height;
            mainPosition->Format = eTextureFormat::RGBA32;
            mainPosition->InitializeData = false;
            mainPosition->EnableRenderTarget = true;
            mainPosition->SampleCount = useMSAA == core::K_TRUE ? msaaSampleCount : 1;
            mainPosition->Init();

            Texture* mainNormal = new Texture();
            mainNormal->Width = viewport->Width;
            mainNormal->Height = viewport->Height;
            mainNormal->Format = eTextureFormat::RGBA16;
            mainNormal->InitializeData = false;
            mainNormal->EnableRenderTarget = true;
            mainNormal->SampleCount = useMSAA == core::K_TRUE ? msaaSampleCount : 1;
            mainNormal->Init();

            m_OpaqueRenderTarget = new RenderTarget({ mainColor, mainPosition, mainNormal }, m_SharedDepthTexture, *viewport);
        
            // Transparent render target
            Texture* mainAccum = new Texture();
            mainAccum->Width = viewport->Width;
            mainAccum->Height = viewport->Height;
            mainAccum->Format = eTextureFormat::RGBA16;
            mainAccum->InitializeData = false;
            mainAccum->EnableRenderTarget = true;
            mainAccum->SampleCount = useMSAA == core::K_TRUE ? msaaSampleCount : 1;
            mainAccum->Init();

            Texture* mainReveal = new Texture();
            mainReveal->Width = viewport->Width;
            mainReveal->Height = viewport->Height;
            mainReveal->Format = eTextureFormat::R8;
            mainReveal->InitializeData = false;
            mainReveal->EnableRenderTarget = true;
            mainReveal->SampleCount = useMSAA == core::K_TRUE ? msaaSampleCount : 1;
            mainReveal->Init();

            m_TransparentRenderTarget = new RenderTarget({ mainAccum, mainReveal }, m_SharedDepthTexture, *viewport);
        }

        void RenderManager::Free()
        {
            delete m_CameraBuffer;
            delete m_DirectLightBuffer;
            delete m_PointLightBuffer;
            delete m_SpotLightBuffer;
        }

        void RenderManager::Render(Scene* scene)
        {
            GeometryManager::BindIndexBuffer();
            FlushLights(scene);

            // Opaque
            m_OpaqueRenderTarget->ClearColor(0, glm::vec4(0.0f));
            m_OpaqueRenderTarget->ClearColor(1, glm::vec4(0.0f));
            m_OpaqueRenderTarget->ClearColor(2, glm::vec4(0.0f));
            m_OpaqueRenderTarget->ClearDepth(1.0f);
            for (RenderPass* rp : *m_OpaqueRenderPasses)
            {
                rp->Bind();
                rp->Draw(scene);
                rp->Unbind();
            }

            // Transparent
            m_TransparentRenderTarget->ClearColor(0, glm::vec4(0.0f));
            m_TransparentRenderTarget->ClearColor(1, glm::vec4(0.0f));
            m_TransparentRenderTarget->ClearDepth(1.0f);
            for (RenderPass* rp : *m_TransparentRenderPasses)
            {
                rp->Bind();
                rp->Draw(scene);
                rp->Unbind();
            }

            // GUI
            for (RenderPass* rp : *m_GUIRenderPasses)
            {
                rp->Bind();
                rp->Draw(scene);
                rp->Unbind();
            }
        }

        void RenderManager::FlushLights(Scene* scene)
        {
            m_DirectLightBuffer->Clear();
            scene->EachComponent<DirectLightComponent>([](DirectLightComponent* component)
            {
                DirectLightBufferData light;
                light.Position = component->Entity->Transform.Position;
                light.Color = component->Color;
                m_DirectLightBuffer->Add(light);
            });
            m_DirectLightBuffer->Flush();

            m_PointLightBuffer->Clear();
            scene->EachComponent<PointLightComponent>([](PointLightComponent* component)
            {
                PointLightBufferData light;
                light.Position = component->Entity->Transform.Position;
                light.Color = component->Color;
                light.Constant = component->Constant;
                light.Linear = component->Constant;
                light.Quadratic = component->Constant;
                m_PointLightBuffer->Add(light);
            });
            m_PointLightBuffer->Flush();

            m_SpotLightBuffer->Clear();
            scene->EachComponent<SpotLightComponent>([](SpotLightComponent* component)
            {
                SpotLightBufferData light;
                light.Position = component->Entity->Transform.Position;
                light.Color = component->Color;
                light.Direction = component->Direction;
                light.Outer = component->Outer;
                light.Cutoff = component->Cutoff;
                m_SpotLightBuffer->Add(light);
            });
            m_SpotLightBuffer->Flush();
        }

    }

}
#include <rendering/render_system.h>
#include <rendering/material/material_manager.h>
#include <rendering/geometry/geometry_manager.h>

#include <anim/skeleton_manager.h>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        RenderSystem::RenderSystem(const Viewport& viewport, u32 sampleCount)
        {
            context::Init();
            ShaderManager::Init();
            GeometryManager::Init();
            MaterialManager::Init();
            SkeletonManager::Init();
            InitBuffers(viewport, sampleCount);
            InitSamplers(viewport, sampleCount);
            InitRenderTargets(viewport, sampleCount);
        }

        RenderSystem::~RenderSystem()
        {
            FreeBuffers();
            FreeSamplers();
            FreeRenderTargets();
            SkeletonManager::Free();
            MaterialManager::Free();
            GeometryManager::Free();
            ShaderManager::Free();
            context::Free();
        }

        void RenderSystem::InitBuffers(const Viewport &viewport, u32 sampleCount)
        {
            m_ViewportBuffer = new ViewportBuffer();
            m_ViewportBuffer->Add(viewport);
            m_ViewportBuffer->Flush();
            m_MonitorBuffer = new MonitorBuffer();
            m_CameraBuffer = new CameraBuffer();
            m_DirectLightBuffer = new DirectLightBuffer();
            m_DirectLightBuffer->Reserve(1000);
            m_PointLightBuffer = new PointLightBuffer();
            m_PointLightBuffer->Reserve(1000);
            m_SpotLightBuffer = new SpotLightBuffer();
            m_SpotLightBuffer->Reserve(1000);
            m_ShadowFilterBuffer = new ShadowFilterBuffer();
        }

        void RenderSystem::InitSamplers(const Viewport &viewport, u32 sampleCount)
        {
            m_ShadowSampler.BorderColor = glm::vec4(1, 1, 1, 1);
            m_ShadowSampler.AddressU = TextureSampler::eAddress::CLAMP;
            m_ShadowSampler.AddressV = TextureSampler::eAddress::CLAMP;
            m_ShadowSampler.Slot = K_SLOT_SHADOW_SAMPLER;

            context::CreateSampler(m_ShadowSampler);
        }

        void RenderSystem::InitRenderTargets(const Viewport& viewport, u32 sampleCount)
        {
            // Final render target
            Texture* finalColor = new Texture();
            finalColor->Width = viewport.Width;
            finalColor->Height = viewport.Height;
            finalColor->Format = eTextureFormat::RGBA8;
            finalColor->InitializeData = false;
            finalColor->EnableRenderTarget = true;
            finalColor->Init();

            Texture* finalDepth = new Texture();
            finalDepth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            finalDepth->Width = viewport.Width;
            finalDepth->Height = viewport.Height;
            finalDepth->Format = eTextureFormat::R32_TYPELESS;
            finalDepth->InitializeData = false;
            finalDepth->EnableRenderTarget = true;
            finalDepth->Init();

            m_FinalRenderTarget = new RenderTarget({ finalColor }, finalDepth, m_ViewportBuffer->GetList());

            // Scene render target
            Texture* sceneColor = new Texture();
            sceneColor->Width = viewport.Width;
            sceneColor->Height = viewport.Height;
            sceneColor->Format = eTextureFormat::RGBA8;
            sceneColor->InitializeData = false;
            sceneColor->EnableRenderTarget = true;
            sceneColor->Init();

            Texture* sceneDepth = new Texture();
            sceneDepth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            sceneDepth->Width = viewport.Width;
            sceneDepth->Height = viewport.Height;
            sceneDepth->Format = eTextureFormat::R32_TYPELESS;
            sceneDepth->InitializeData = false;
            sceneDepth->EnableRenderTarget = true;
            sceneDepth->Init();

            m_SceneRenderTarget = new RenderTarget({ sceneColor }, sceneDepth, m_ViewportBuffer->GetList());

            // Shadow map as a depth stencil texture output for shadow mapping
            Texture* shadowDepth = new Texture();
            shadowDepth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            shadowDepth->Width = viewport.Width;
            shadowDepth->Height = viewport.Height;
            shadowDepth->Format = eTextureFormat::R32_TYPELESS;
            shadowDepth->InitializeData = false;
            shadowDepth->EnableRenderTarget = true;
            shadowDepth->SampleCount = 1;
            shadowDepth->Slot = K_SLOT_SHADOW_MAP;
            shadowDepth->Init();

            m_ShadowRenderTarget = new RenderTarget(shadowDepth, viewport);

            // Shared depth texture for opaque and transparent render targets
            m_SharedDepthTexture = new Texture();
            m_SharedDepthTexture->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            m_SharedDepthTexture->Width = viewport.Width;
            m_SharedDepthTexture->Height = viewport.Height;
            m_SharedDepthTexture->Format = eTextureFormat::R32_TYPELESS;
            m_SharedDepthTexture->InitializeData = false;
            m_SharedDepthTexture->EnableRenderTarget = true;
            m_SharedDepthTexture->SampleCount = sampleCount;
            m_SharedDepthTexture->Init();

            // Opaque render target
            Texture* opaqueColor = new Texture();
            opaqueColor->Width = viewport.Width;
            opaqueColor->Height = viewport.Height;
            opaqueColor->Format = eTextureFormat::HDR;
            opaqueColor->InitializeData = false;
            opaqueColor->EnableRenderTarget = true;
            opaqueColor->SampleCount = sampleCount;
            opaqueColor->Init();

            Texture* opaquePosition = new Texture();
            opaquePosition->Width = viewport.Width;
            opaquePosition->Height = viewport.Height;
            opaquePosition->Format = eTextureFormat::RGBA32;
            opaquePosition->InitializeData = false;
            opaquePosition->EnableRenderTarget = true;
            opaquePosition->SampleCount = sampleCount;
            opaquePosition->Init();

            Texture* opaqueNormal = new Texture();
            opaqueNormal->Width = viewport.Width;
            opaqueNormal->Height = viewport.Height;
            opaqueNormal->Format = eTextureFormat::RGBA16;
            opaqueNormal->InitializeData = false;
            opaqueNormal->EnableRenderTarget = true;
            opaqueNormal->SampleCount = sampleCount;
            opaqueNormal->Init();

            m_OpaqueRenderTarget = new RenderTarget({ opaqueColor, opaquePosition, opaqueNormal }, m_SharedDepthTexture, viewport);

            // Transparent render target
            Texture* transparentAccum = new Texture();
            transparentAccum->Width = viewport.Width;
            transparentAccum->Height = viewport.Height;
            transparentAccum->Format = eTextureFormat::HDR;
            transparentAccum->InitializeData = false;
            transparentAccum->EnableRenderTarget = true;
            transparentAccum->SampleCount = sampleCount;
            transparentAccum->Init();

            Texture* transparentReveal = new Texture();
            transparentReveal->Width = viewport.Width;
            transparentReveal->Height = viewport.Height;
            transparentReveal->Format = eTextureFormat::R8;
            transparentReveal->InitializeData = false;
            transparentReveal->EnableRenderTarget = true;
            transparentReveal->SampleCount = sampleCount;
            transparentReveal->Init();

            m_TransparentRenderTarget = new RenderTarget({ transparentAccum, transparentReveal }, m_SharedDepthTexture, viewport);

            // UI render target
            Texture* uiColor = new Texture();
            uiColor->Width = viewport.Width;
            uiColor->Height = viewport.Height;
            uiColor->Format = eTextureFormat::RGBA8;
            uiColor->InitializeData = false;
            uiColor->EnableRenderTarget = true;
            uiColor->Init();

            Texture* uiDepth = new Texture();
            uiDepth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            uiDepth->Width = viewport.Width;
            uiDepth->Height = viewport.Height;
            uiDepth->Format = eTextureFormat::R32_TYPELESS;
            uiDepth->InitializeData = false;
            uiDepth->EnableRenderTarget = true;
            uiDepth->Init();

            m_UiRenderTarget = new RenderTarget({ uiColor }, uiDepth, viewport);
        }

        void RenderSystem::FreeBuffers()
        {
            delete m_ViewportBuffer;
            delete m_MonitorBuffer;
            delete m_CameraBuffer;
            delete m_DirectLightBuffer;
            delete m_PointLightBuffer;
            delete m_SpotLightBuffer;
            delete m_ShadowFilterBuffer;
        }

        void RenderSystem::FreeSamplers()
        {
            context::FreeSampler(m_ShadowSampler);
        }

        void RenderSystem::FreeRenderTargets()
        {
            delete m_FinalRenderTarget;
            delete m_SceneRenderTarget;
            delete m_ShadowRenderTarget;
            delete m_OpaqueRenderTarget;
            delete m_TransparentRenderTarget;
            delete m_UiRenderTarget;
        }

        void RenderSystem::Prepare()
        {
            GeometryManager::Bind();
        }

        void RenderSystem::Update(Scene* scene, const Time& dt)
        {
            UpdateLight(scene);
            UpdatePasses(scene);
        }

        void RenderSystem::UpdateLight(Scene* scene)
        {
            m_DirectLightBuffer->Clear();
            scene->EachComponent<DirectLightComponent>([this](DirectLightComponent* component)
            {
                if (component->FollowEntity) {
                    component->Position = component->Entity->Transform.Position;
                }

                DirectLightData light;
                light.Position = component->Position;
                light.Color = component->Color;
                m_DirectLightBuffer->Add(light);
            });
            m_DirectLightBuffer->Flush();

            m_PointLightBuffer->Clear();
            scene->EachComponent<PointLightComponent>([this](PointLightComponent* component)
            {
                if (component->FollowEntity) {
                    component->Position = component->Entity->Transform.Position;
                }

                PointLightData light;
                light.Position = component->Position;
                light.Color = component->Color;
                light.Constant = component->Constant;
                light.Linear = component->Constant;
                light.Quadratic = component->Constant;
                m_PointLightBuffer->Add(light);
            });
            m_PointLightBuffer->Flush();

            m_SpotLightBuffer->Clear();
            scene->EachComponent<SpotLightComponent>([this](SpotLightComponent* component)
            {
                if (component->FollowEntity) {
                    component->Position = component->Entity->Transform.Position;
                }

                SpotLightData light;
                light.Position = component->Position;
                light.Color = component->Color;
                light.Direction = component->Direction;
                light.Outer = component->Outer;
                light.Cutoff = component->Cutoff;
                m_SpotLightBuffer->Add(light);
            });
            m_SpotLightBuffer->Flush();
        }

        void RenderSystem::UpdatePasses(xpe::ecs::Scene *scene)
        {
            // Shadow
            m_ShadowRenderTarget->ClearDepth(1.0f);
            for (RenderPass* rp : m_ShadowRenderPasses)
            {
                if (rp->Enable) {
                    rp->Update(scene);
                    rp->Bind();
                    rp->DrawShadow(scene);
                    rp->Unbind();
                }
            }

            // Opaque
            m_OpaqueRenderTarget->ClearColor(0, glm::vec4(0.0f));
            m_OpaqueRenderTarget->ClearColor(1, glm::vec4(0.0f));
            m_OpaqueRenderTarget->ClearColor(2, glm::vec4(0.0f));
            m_OpaqueRenderTarget->ClearDepth(1.0f);

            for (RenderPass* rp : m_OpaqueRenderPasses)
            {
                if (rp->Enable) {
                    rp->Update(scene);
                    rp->Bind();
                    rp->DrawOpaque(scene);
                    rp->Unbind();
                }
            }

            // Transparent
            m_TransparentRenderTarget->ClearColor(0, glm::vec4(0.0f));
            m_TransparentRenderTarget->ClearColor(1, glm::vec4(1.0f));

            for (RenderPass* rp : m_TransparentRenderPasses)
            {
                if (rp->Enable) {
                    rp->Update(scene);
                    rp->Bind();
                    rp->DrawTransparent(scene);
                    rp->Unbind();
                }
            }

            // PostFX
            m_SceneRenderTarget->ClearColor(0, glm::vec4(0.0f));
            m_SceneRenderTarget->ClearDepth(1.0f);
            for (RenderPass* rp : m_PostFXRenderPasses)
            {
                if (rp->Enable) {
                    rp->Update(scene);
                    rp->Bind();
                    rp->DrawPostFX(scene);
                    rp->Unbind();
                }
            }

            // UI
            m_UiRenderTarget->ClearColor(0, glm::vec4(0.0f));
            m_UiRenderTarget->ClearDepth(1.0f);
            for (RenderPass* rp : m_UiRenderPasses)
            {
                if (rp->Enable) {
                    rp->Update(scene);
                    rp->Bind();
                    rp->DrawUI(scene);
                    rp->Unbind();
                }
            }

            // Final
            m_FinalRenderTarget->ClearColor(0, glm::vec4(0.0f));
            m_FinalRenderTarget->ClearDepth(1.0f);
            for (RenderPass* rp : m_FinalRenderPasses)
            {
                if (rp->Enable) {
                    rp->Update(scene);
                    rp->Bind();
                    rp->DrawFinal(scene);
                    rp->Unbind();
                }
            }
        }

    }

}
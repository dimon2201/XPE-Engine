#include <rendering/render_system.h>
#include <rendering/material/material_manager.h>
#include <rendering/geometry/geometry_manager.h>
#include <rendering/skybox_manager.h>
#include <rendering/camera_manager.h>

#include <anim/skeleton_manager.h>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        cRenderSystem::cRenderSystem(sViewport& viewport, u32 sampleCount)
        {
            context::Init();
            InitManagers(viewport, sampleCount);
            cCameraManager::SetViewport(viewport);
            InitBuffers(*cCameraManager::GetViewport(), sampleCount);
            InitSamplers(*cCameraManager::GetViewport(), sampleCount);
            InitRenderTargets(*cCameraManager::GetViewport(), sampleCount);
            AddWindowFrameResized(cRenderSystem, eWindowFrameResizedPriority::RENDER_SYSTEM);
        }

        cRenderSystem::~cRenderSystem()
        {
            FreeBuffers();
            FreeSamplers();
            FreeRenderTargets();
            FreeManagers();
            context::Free();
            RemoveWindowFrameResized();
        }

        void cRenderSystem::InitManagers(sViewport &viewport, u32 sampleCount)
        {
            cShaderManager::Init();
            cCameraManager::Init();
            cGeometryManager::Init();
            cMaterialManager::Init();
            cSkeletonManager::Init();
            cSkyboxManager::Init();
        }

        void cRenderSystem::InitBuffers(sViewport &viewport, u32 sampleCount)
        {
            m_DirectLightBuffer = new sDirectLightBuffer();
            m_DirectLightBuffer->Reserve(10);
            m_DirectLightMatrixBuffer = new sDirectLightMatrixBuffer();
            m_DirectLightMatrixBuffer->Reserve(10);
            m_PointLightBuffer = new sPointLightBuffer();
            m_PointLightBuffer->Reserve(10);
            m_SpotLightBuffer = new sSpotLightBuffer();
            m_SpotLightBuffer->Reserve(10);
        }

        void cRenderSystem::InitSamplers(sViewport &viewport, u32 sampleCount)
        {
            m_ShadowSampler.Filter      = sSampler::eFilter::MIN_MAG_MIP_POINT;
            m_ShadowSampler.BorderColor = glm::vec4(1, 1, 1, 1);
            m_ShadowSampler.AddressU    = sSampler::eAddress::BORDER;
            m_ShadowSampler.AddressV    = sSampler::eAddress::BORDER;
            m_ShadowSampler.Slot        = K_SLOT_SHADOW_SAMPLER;

            context::CreateSampler(m_ShadowSampler);
        }

        void cRenderSystem::InitRenderTargets(sViewport& viewport, u32 sampleCount)
        {
            // Final render target
            sTexture* finalColor = new sTexture();
            finalColor->Width = viewport.Width;
            finalColor->Height = viewport.Height;
            finalColor->Format = eTextureFormat::RGBA8;
            finalColor->InitializeData = false;
            finalColor->EnableRenderTarget = true;
            finalColor->SetResizable(true);
            finalColor->Init();

            sTexture* finalDepth = new sTexture();
            finalDepth->Type = sTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            finalDepth->Width = viewport.Width;
            finalDepth->Height = viewport.Height;
            finalDepth->Format = eTextureFormat::R32_TYPELESS;
            finalDepth->InitializeData = false;
            finalDepth->EnableRenderTarget = true;
            finalDepth->SetResizable(true);
            finalDepth->Init();

            m_FinalRenderTarget = new sRenderTarget({finalColor }, finalDepth, &viewport);
            m_FinalRenderTarget->SetResizable(true);

            // Scene render target
            sTexture* sceneColor = new sTexture();
            sceneColor->Width = viewport.Width;
            sceneColor->Height = viewport.Height;
            sceneColor->Format = eTextureFormat::RGBA8;
            sceneColor->InitializeData = false;
            sceneColor->EnableRenderTarget = true;
            sceneColor->SetResizable(true);
            sceneColor->Init();

            sTexture* sceneDepth = new sTexture();
            sceneDepth->Type = sTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            sceneDepth->Width = viewport.Width;
            sceneDepth->Height = viewport.Height;
            sceneDepth->Format = eTextureFormat::R32_TYPELESS;
            sceneDepth->InitializeData = false;
            sceneDepth->EnableRenderTarget = true;
            sceneDepth->SetResizable(true);
            sceneDepth->Init();

            m_SceneRenderTarget = new sRenderTarget({ sceneColor }, sceneDepth, &viewport);
            m_SceneRenderTarget->SetResizable(true);

            // Shadow render target
            sTexture* shadowColor = new sTexture();
            shadowColor->Width = 1024;
            shadowColor->Height = 1024;
            shadowColor->Format = eTextureFormat::RGBA8;
            shadowColor->InitializeData = false;
            shadowColor->EnableRenderTarget = true;
            shadowColor->Slot = K_SLOT_SHADOW_ATLAS;
            shadowColor->SetResizable(true);
            shadowColor->Init();

            sTexture* shadowDepth = new sTexture();
            shadowDepth->Type = sTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            shadowDepth->Width = 1024;
            shadowDepth->Height = 1024;
            shadowDepth->Format = eTextureFormat::R32_TYPELESS;
            shadowDepth->InitializeData = false;
            shadowDepth->EnableRenderTarget = true;
            shadowDepth->Slot = K_SLOT_SHADOW_ATLAS;
            shadowDepth->SetResizable(true);
            shadowDepth->Init();

            m_ShadowRenderTarget = new sRenderTarget({ shadowColor }, shadowDepth, &viewport);
            m_ShadowRenderTarget->SetResizable(true);

            // Shared depth texture for opaque and transparent render targets
            m_SharedDepthTexture = new sTexture();
            m_SharedDepthTexture->Type = sTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            m_SharedDepthTexture->Width = viewport.Width;
            m_SharedDepthTexture->Height = viewport.Height;
            m_SharedDepthTexture->Format = eTextureFormat::R32_TYPELESS;
            m_SharedDepthTexture->InitializeData = false;
            m_SharedDepthTexture->EnableRenderTarget = true;
            m_SharedDepthTexture->SampleCount = sampleCount;
            m_SharedDepthTexture->SetResizable(false);
            m_SharedDepthTexture->Init();

            // Opaque render target
            sTexture* opaqueColor = new sTexture();
            opaqueColor->Width = viewport.Width;
            opaqueColor->Height = viewport.Height;
            opaqueColor->Format = eTextureFormat::HDR;
            opaqueColor->InitializeData = false;
            opaqueColor->EnableRenderTarget = true;
            opaqueColor->SampleCount = sampleCount;
            opaqueColor->SetResizable(false);
            opaqueColor->Init();

            sTexture* opaquePosition = new sTexture();
            opaquePosition->Width = viewport.Width;
            opaquePosition->Height = viewport.Height;
            opaquePosition->Format = eTextureFormat::RGBA32;
            opaquePosition->InitializeData = false;
            opaquePosition->EnableRenderTarget = true;
            opaquePosition->SampleCount = sampleCount;
            opaquePosition->SetResizable(false);
            opaquePosition->Init();

            sTexture* opaqueNormal = new sTexture();
            opaqueNormal->Width = viewport.Width;
            opaqueNormal->Height = viewport.Height;
            opaqueNormal->Format = eTextureFormat::RGBA16;
            opaqueNormal->InitializeData = false;
            opaqueNormal->EnableRenderTarget = true;
            opaqueNormal->SampleCount = sampleCount;
            opaqueNormal->SetResizable(false);
            opaqueNormal->Init();

            m_OpaqueRenderTarget = new sRenderTarget({opaqueColor, opaquePosition, opaqueNormal }, m_SharedDepthTexture, &viewport);
            m_OpaqueRenderTarget->SetResizable(false);

            // Transparent render target
            sTexture* transparentAccum = new sTexture();
            transparentAccum->Width = viewport.Width;
            transparentAccum->Height = viewport.Height;
            transparentAccum->Format = eTextureFormat::HDR;
            transparentAccum->InitializeData = false;
            transparentAccum->EnableRenderTarget = true;
            transparentAccum->SampleCount = sampleCount;
            transparentAccum->SetResizable(false);
            transparentAccum->Init();

            sTexture* transparentReveal = new sTexture();
            transparentReveal->Width = viewport.Width;
            transparentReveal->Height = viewport.Height;
            transparentReveal->Format = eTextureFormat::R8;
            transparentReveal->InitializeData = false;
            transparentReveal->EnableRenderTarget = true;
            transparentReveal->SampleCount = sampleCount;
            transparentReveal->SetResizable(false);
            transparentReveal->Init();

            m_TransparentRenderTarget = new sRenderTarget({transparentAccum, transparentReveal }, m_SharedDepthTexture, &viewport);
            m_TransparentRenderTarget->SetResizable(false);

            // UI render target
            sTexture* uiColor = new sTexture();
            uiColor->Width = viewport.Width;
            uiColor->Height = viewport.Height;
            uiColor->Format = eTextureFormat::RGBA8;
            uiColor->InitializeData = false;
            uiColor->EnableRenderTarget = true;
            uiColor->SetResizable(true);
            uiColor->Init();

            sTexture* uiDepth = new sTexture();
            uiDepth->Type = sTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            uiDepth->Width = viewport.Width;
            uiDepth->Height = viewport.Height;
            uiDepth->Format = eTextureFormat::R32_TYPELESS;
            uiDepth->InitializeData = false;
            uiDepth->EnableRenderTarget = true;
            uiDepth->SetResizable(true);
            uiDepth->Init();

            m_UiRenderTarget = new sRenderTarget({ uiColor }, uiDepth, &viewport);
            m_UiRenderTarget->SetResizable(true);
        }

        void cRenderSystem::FreeManagers()
        {
            cSkyboxManager::Free();
            cSkeletonManager::Free();
            cMaterialManager::Free();
            cGeometryManager::Free();
            cCameraManager::Free();
            cShaderManager::Free();
        }

        void cRenderSystem::FreeBuffers()
        {
            delete m_DirectLightBuffer;
            delete m_DirectLightMatrixBuffer;
            delete m_PointLightBuffer;
            delete m_SpotLightBuffer;
        }

        void cRenderSystem::FreeSamplers()
        {
            context::FreeSampler(m_ShadowSampler);
        }

        void cRenderSystem::FreeRenderTargets()
        {
            delete m_FinalRenderTarget;
            delete m_SceneRenderTarget;
            delete m_ShadowRenderTarget;
            delete m_OpaqueRenderTarget;
            delete m_TransparentRenderTarget;
            delete m_UiRenderTarget;
        }

        void cRenderSystem::WindowFrameResized(int width, int height)
        {
            sViewport& viewport = cCameraManager::GetBuffer()->Item.Viewport;
            viewport.Width = width;
            viewport.Height = height;
            cCameraManager::Flush();

            m_SharedDepthTexture->Resize(width, height);

            m_OpaqueRenderTarget->DepthStencil = m_SharedDepthTexture;
            m_OpaqueRenderTarget->ResizeColors(width, height);
            m_OpaqueRenderTarget->Resize(width, height);

            m_TransparentRenderTarget->DepthStencil = m_SharedDepthTexture;
            m_TransparentRenderTarget->ResizeColors(width, height);
            m_TransparentRenderTarget->Resize(width, height);
        }

        void cRenderSystem::Prepare()
        {
            cGeometryManager::Bind();
        }

        void cRenderSystem::Update(cScene* scene, const cTime& dt)
        {
            cSkeletonManager::Flush();
            UpdateLight(scene);
            UpdatePasses(scene);
        }

        void cRenderSystem::UpdateLight(cScene* scene)
        {
            m_DirectLightMatrixBuffer->Clear();

            m_DirectLightBuffer->Clear();
            scene->ForEach<sCDirectionalLight>([this](sCDirectionalLight *component) {
                if (component->FollowEntity) {
                    component->View.Position = component->Entity->GetPosition();
                }

                sDirectLightData light;
                light.Position = component->View.Position;
                light.Color = component->Color;

                sDirectLightMatrix lightMatrix;
                lightMatrix.Matrix = cMathManager::UpdateDirectLightMatrix(component->Projection, component->View);

                m_DirectLightBuffer->Add(light);
                m_DirectLightMatrixBuffer->Add(lightMatrix);
            });
            m_DirectLightBuffer->Flush();

            m_SpotLightBuffer->Clear();
            scene->ForEach<sCSpotLight>([this](sCSpotLight *component) {
                if (component->FollowEntity) {
                    component->View.Position = component->Entity->GetPosition();
                }

                sSpotLightData light;
                light.Position = component->View.Position;
                light.Color = component->Color;
                light.Direction = component->View.Front;
                light.Outer = component->Outer;
                light.Cutoff = component->Cutoff;

                sDirectLightMatrix lightMatrix;
                lightMatrix.Matrix = cMathManager::UpdateDirectLightMatrix(component->Projection, component->View);

                m_SpotLightBuffer->Add(light);
                m_DirectLightMatrixBuffer->Add(lightMatrix);
            });
            m_SpotLightBuffer->Flush();

            m_DirectLightMatrixBuffer->Flush();

            m_PointLightBuffer->Clear();
            scene->ForEach<sCPointLight>([this](sCPointLight *component) {
                if (component->FollowEntity) {
                    component->Position = component->Entity->GetPosition();
                }

                sPointLightData light;
                light.Position = component->Position;
                light.Color = component->Color;
                light.Constant = component->Constant;
                light.Linear = component->Constant;
                light.Quadratic = component->Constant;
                m_PointLightBuffer->Add(light);
            });
            m_PointLightBuffer->Flush();
        }

        void cRenderSystem::UpdatePasses(xpe::ecs::cScene *scene)
        {
            // Shadow
            m_ShadowRenderTarget->ClearColor(0, glm::vec4(1.0f));
            m_ShadowRenderTarget->ClearDepth(1.0f);
            for (cRenderPass* rp : m_ShadowRenderPasses)
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
            for (cRenderPass* rp : m_OpaqueRenderPasses)
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
            for (cRenderPass* rp : m_TransparentRenderPasses)
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
            for (cRenderPass* rp : m_PostFXRenderPasses)
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
            for (cRenderPass* rp : m_UiRenderPasses)
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
            for (cRenderPass* rp : m_FinalRenderPasses)
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
#include <rendering/render_system.h>
#include <rendering/material/material_manager.h>
#include <rendering/geometry/geometry_manager.h>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        RenderSystem::RenderSystem(const Viewport& viewport, u32 sampleCount)
        {
            context::Init();

            ShaderManager::Init();
            GeometryManager::Init();
            MaterialManager::Init();

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

            m_ShadowSampler = new TextureSampler();
            m_ShadowSampler->BorderColor = glm::vec4(1, 1, 1, 1);
            m_ShadowSampler->AddressU = TextureSampler::eAddress::CLAMP;
            m_ShadowSampler->AddressV = TextureSampler::eAddress::CLAMP;
            m_ShadowSampler->Slot = K_SLOT_SHADOW_SAMPLER;

            m_TransparentRenderPasses.reserve(2);

            InitRenderTargets(viewport, sampleCount);
        }

        RenderSystem::~RenderSystem()
        {
            delete m_ViewportBuffer;
            delete m_MonitorBuffer;
            delete m_CameraBuffer;
            delete m_DirectLightBuffer;
            delete m_PointLightBuffer;
            delete m_SpotLightBuffer;
            delete m_ShadowFilterBuffer;

            delete m_ShadowSampler;

            delete m_CanvasRenderTarget;
            delete m_OpaqueRenderTarget;
            delete m_TransparentRenderTarget;
            delete m_ShadowRenderTarget;

            MaterialManager::Free();
            GeometryManager::Free();
            ShaderManager::Free();

            context::Free();
        }

        void RenderSystem::InitRenderTargets(const Viewport& viewport, u32 sampleCount)
        {
            // Canvas render target
            Texture* canvasColor = new Texture();
            canvasColor->Width = viewport.Width;
            canvasColor->Height = viewport.Height;
            canvasColor->Format = eTextureFormat::RGBA8;
            canvasColor->InitializeData = false;
            canvasColor->EnableRenderTarget = true;
            canvasColor->Init();

            Texture* canvasDepth = new Texture();
            canvasDepth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            canvasDepth->Width = viewport.Width;
            canvasDepth->Height = viewport.Height;
            canvasDepth->Format = eTextureFormat::R32_TYPELESS;
            canvasDepth->InitializeData = false;
            canvasDepth->EnableRenderTarget = true;
            canvasDepth->Init();

            m_CanvasRenderTarget = new RenderTarget({ canvasColor }, canvasDepth, m_ViewportBuffer->GetList());

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
            Texture* mainColor = new Texture();
            mainColor->Width = viewport.Width;
            mainColor->Height = viewport.Height;
            mainColor->Format = eTextureFormat::RGBA8;
            mainColor->InitializeData = false;
            mainColor->EnableRenderTarget = true;
            mainColor->SampleCount = sampleCount;
            mainColor->Init();

            Texture* mainPosition = new Texture();
            mainPosition->Width = viewport.Width;
            mainPosition->Height = viewport.Height;
            mainPosition->Format = eTextureFormat::RGBA32;
            mainPosition->InitializeData = false;
            mainPosition->EnableRenderTarget = true;
            mainPosition->SampleCount = sampleCount;
            mainPosition->Init();

            Texture* mainNormal = new Texture();
            mainNormal->Width = viewport.Width;
            mainNormal->Height = viewport.Height;
            mainNormal->Format = eTextureFormat::RGBA16;
            mainNormal->InitializeData = false;
            mainNormal->EnableRenderTarget = true;
            mainNormal->SampleCount = sampleCount;
            mainNormal->Init();

            m_OpaqueRenderTarget = new RenderTarget({ mainColor, mainPosition, mainNormal }, m_SharedDepthTexture, viewport);

            // Transparent render target
            Texture* mainAccum = new Texture();
            mainAccum->Width = viewport.Width;
            mainAccum->Height = viewport.Height;
            mainAccum->Format = eTextureFormat::RGBA16;
            mainAccum->InitializeData = false;
            mainAccum->EnableRenderTarget = true;
            mainAccum->SampleCount = sampleCount;
            mainAccum->Init();

            Texture* mainReveal = new Texture();
            mainReveal->Width = viewport.Width;
            mainReveal->Height = viewport.Height;
            mainReveal->Format = eTextureFormat::R8;
            mainReveal->InitializeData = false;
            mainReveal->EnableRenderTarget = true;
            mainReveal->SampleCount = sampleCount;
            mainReveal->Init();

            m_TransparentRenderTarget = new RenderTarget({ mainAccum, mainReveal }, m_SharedDepthTexture, viewport);

            // Shadow map as a depth stencil texture output for shadow mapping
            Texture* shadowMap = new Texture();
            shadowMap->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            shadowMap->Width = viewport.Width;
            shadowMap->Height = viewport.Height;
            shadowMap->Format = eTextureFormat::R32_TYPELESS;
            shadowMap->InitializeData = false;
            shadowMap->EnableRenderTarget = true;
            shadowMap->SampleCount = 1;
            shadowMap->Slot = K_SLOT_SHADOW_MAP;
            shadowMap->Init();

            m_ShadowRenderTarget = new RenderTarget(shadowMap, viewport);
        }

        void RenderSystem::Prepare()
        {
            GeometryManager::BindVertexBuffer();
            GeometryManager::BindIndexBuffer();
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
            m_TransparentRenderTarget->ClearDepth(1.0f);
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
            for (RenderPass* rp : m_PostFXRenderPasses)
            {
                if (rp->Enable) {
                    rp->Update(scene);
                    rp->Bind();
                    rp->DrawPostFX(scene);
                    rp->Unbind();
                }
            }
        }

    }

}
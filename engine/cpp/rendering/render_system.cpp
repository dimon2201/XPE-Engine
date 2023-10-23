#include <rendering/render_system.h>
#include <rendering/passes/render_pass.h>
#include <rendering/passes/canvas.hpp>
#include <rendering/monitor.h>
#include <rendering/buffers/camera_buffer.h>
#include <rendering/buffers/light_buffers.h>
#include <rendering/buffers/shadow_filter_buffer.h>
#include <rendering/material/material_manager.h>
#include <rendering/geometry/geometry_manager.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        RenderSystem::RenderSystem()
        {
            context::Init();
            ShaderManager::Init();
            Monitor::Init();
            GeometryManager::Init();
            MaterialManager::Init();

            m_CameraBuffer = new render::CameraBuffer();

            m_DirectLightBuffer = new render::DirectLightBuffer();
            m_DirectLightBuffer->Reserve(1000);

            m_PointLightBuffer = new render::PointLightBuffer();
            m_PointLightBuffer->Reserve(1000);

            m_SpotLightBuffer = new render::SpotLightBuffer();
            m_SpotLightBuffer->Reserve(1000);

            m_ShadowSampler = new TextureSampler();
            m_ShadowMap = new Texture();
            m_ShadowCoords = new Texture();
            m_ShadowFilterBuffer = new render::ShadowFilterBuffer();
        }

        RenderSystem::~RenderSystem()
        {
            delete m_CameraBuffer;

            delete m_DirectLightBuffer;
            delete m_PointLightBuffer;
            delete m_SpotLightBuffer;

            delete m_ShadowSampler;
            delete m_ShadowMap;
            delete m_ShadowCoords;
            delete m_ShadowFilterBuffer;

            MaterialManager::Free();
            GeometryManager::Free();
            Monitor::Free();
            ShaderManager::Free();
            context::Free();
        }

        void RenderSystem::InitRenderTargets(render::Canvas* canvas)
        {
            m_Canvas = canvas;
            Texture* mainColor = new Texture();
            mainColor->Width = m_Canvas->GetDimension().x;
            mainColor->Height = m_Canvas->GetDimension().y;
            mainColor->Format = eTextureFormat::RGBA8;
            mainColor->InitializeData = false;
            mainColor->EnableRenderTarget = true;
            mainColor->Init();

            Texture* mainPosition = new Texture();
            mainPosition->Width = m_Canvas->GetDimension().x;
            mainPosition->Height = m_Canvas->GetDimension().y;
            mainPosition->Format = eTextureFormat::RGBA32;
            mainPosition->InitializeData = false;
            mainPosition->EnableRenderTarget = true;
            mainPosition->Init();

            Texture* mainNormal = new Texture();
            mainNormal->Width = m_Canvas->GetDimension().x;
            mainNormal->Height = m_Canvas->GetDimension().y;
            mainNormal->Format = eTextureFormat::RGBA16;
            mainNormal->InitializeData = false;
            mainNormal->EnableRenderTarget = true;
            mainNormal->Init();

            Texture* mainDepth = new Texture();
            mainDepth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            mainDepth->Width = m_Canvas->GetDimension().x;
            mainDepth->Height = m_Canvas->GetDimension().y;
            mainDepth->Format = eTextureFormat::R32_TYPELESS;
            mainDepth->InitializeData = false;
            mainDepth->EnableRenderTarget = true;
            mainDepth->Init();

            m_MainRT = new RenderTarget({ mainColor, mainPosition, mainNormal }, mainDepth, *m_Canvas->GetViewport(0));
        }

        void RenderSystem::Prepare()
        {
            GeometryManager::BindVertexBuffer();
            GeometryManager::BindIndexBuffer();
        }

        void RenderSystem::Update(Scene* scene, const Time& dt)
        {
            m_MainRT->ClearColor(0, { 1, 1, 1, 1 });
            m_MainRT->ClearColor(1, { 0, 0, 0, 0 });
            m_MainRT->ClearColor(2, { 0, 0, 0, 0 });
            m_MainRT->ClearDepth(1);
            UpdateLight(scene);
            UpdatePasses(scene);
        }

        void RenderSystem::UpdateLight(Scene* scene)
        {
            this->m_DirectLightBuffer->Clear();
            scene->EachComponent<DirectLightComponent>([this](DirectLightComponent* component)
            {
                DirectLightBufferData light;
                light.Position = component->Entity->Transform.Position;
                light.Color = component->Color;
                this->m_DirectLightBuffer->Add(light);
            });
            this->m_DirectLightBuffer->Flush();

            this->m_PointLightBuffer->Clear();
            scene->EachComponent<PointLightComponent>([this](PointLightComponent* component)
            {
                PointLightBufferData light;
                light.Position = component->Entity->Transform.Position;
                light.Color = component->Color;
                light.Constant = component->Constant;
                light.Linear = component->Constant;
                light.Quadratic = component->Constant;
                this->m_PointLightBuffer->Add(light);
            });
            this->m_PointLightBuffer->Flush();

            this->m_SpotLightBuffer->Clear();
            scene->EachComponent<SpotLightComponent>([this](SpotLightComponent* component)
            {
                SpotLightBufferData light;
                light.Position = component->Entity->Transform.Position;
                light.Color = component->Color;
                light.Direction = component->Direction;
                light.Outer = component->Outer;
                light.Cutoff = component->Cutoff;
                this->m_SpotLightBuffer->Add(light);
            });
            this->m_SpotLightBuffer->Flush();
        }

        void RenderSystem::UpdatePasses(xpe::ecs::Scene *scene)
        {
            for (RenderPass* rp : m_RenderPasses)
            {
                rp->Update(scene);
                rp->Bind();
                rp->Draw(scene);
                rp->Unbind();
            }
        }

    }

}
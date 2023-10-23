#include <rendering/render_system.h>
#include <rendering/passes/render_pass.h>
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

            CameraBuffer = new render::CameraBuffer();

            DirectLightBuffer = new render::DirectLightBuffer();
            DirectLightBuffer->Reserve(1000);

            PointLightBuffer = new render::PointLightBuffer();
            PointLightBuffer->Reserve(1000);

            SpotLightBuffer = new render::SpotLightBuffer();
            SpotLightBuffer->Reserve(1000);

            ShadowSampler = new TextureSampler();
            ShadowMap = new Texture();
            ShadowCoords = new Texture();
            ShadowFilterBuffer = new render::ShadowFilterBuffer();
        }

        RenderSystem::~RenderSystem()
        {
            delete CameraBuffer;

            delete DirectLightBuffer;
            delete PointLightBuffer;
            delete SpotLightBuffer;

            delete ShadowSampler;
            delete ShadowMap;
            delete ShadowCoords;
            delete ShadowFilterBuffer;

            MaterialManager::Free();
            GeometryManager::Free();
            Monitor::Free();
            ShaderManager::Free();
            context::Free();
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
            this->DirectLightBuffer->Clear();
            scene->EachComponent<DirectLightComponent>([this](DirectLightComponent* component)
            {
                DirectLightBufferData light;
                light.Position = component->Entity->Transform.Position;
                light.Color = component->Color;
                this->DirectLightBuffer->Add(light);
            });
            this->DirectLightBuffer->Flush();

            this->PointLightBuffer->Clear();
            scene->EachComponent<PointLightComponent>([this](PointLightComponent* component)
            {
                PointLightBufferData light;
                light.Position = component->Entity->Transform.Position;
                light.Color = component->Color;
                light.Constant = component->Constant;
                light.Linear = component->Constant;
                light.Quadratic = component->Constant;
                this->PointLightBuffer->Add(light);
            });
            this->PointLightBuffer->Flush();

            this->SpotLightBuffer->Clear();
            scene->EachComponent<SpotLightComponent>([this](SpotLightComponent* component)
            {
                SpotLightBufferData light;
                light.Position = component->Entity->Transform.Position;
                light.Color = component->Color;
                light.Direction = component->Direction;
                light.Outer = component->Outer;
                light.Cutoff = component->Cutoff;
                this->SpotLightBuffer->Add(light);
            });
            this->SpotLightBuffer->Flush();
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
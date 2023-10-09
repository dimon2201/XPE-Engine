#include <rendering/renderer.h>
#include <rendering/passes/render_pass.h>
#include <rendering/monitor.h>
#include <rendering/buffers/camera_buffer.h>
#include <rendering/buffers/light_buffers.h>
#include <rendering/shadow/shadow.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        Renderer::Renderer()
        {
            context::Init();
            ShaderManager::Init();
            GeometryManager::Init();
            Monitor::Init();
            Shadow::Init();

            CameraBuffer = new render::CameraBuffer();

            DirectLightBuffer = new render::DirectLightBuffer();
            DirectLightBuffer->Reserve(1000);

            PointLightBuffer = new render::PointLightBuffer();
            PointLightBuffer->Reserve(1000);

            SpotLightBuffer = new render::SpotLightBuffer();
            SpotLightBuffer->Reserve(1000);
        }

        Renderer::~Renderer()
        {
            delete CameraBuffer;
            delete DirectLightBuffer;
            delete PointLightBuffer;
            delete SpotLightBuffer;

            for (Drawer* drawer : m_Drawers) {
                delete drawer;
            }
            m_Drawers.clear();

            Shadow::Free();
            Monitor::Free();
            GeometryManager::Free();
            ShaderManager::Free();
            context::Free();
        }

        void Renderer::RemoveDrawer(Drawer* drawer)
        {
            auto it = std::find(m_Drawers.begin(), m_Drawers.end(), drawer);
            if (it != m_Drawers.end()) {
                m_Drawers.erase(it);
                delete it.operator->();
            }
        }

        void Renderer::Render(Scene* scene)
        {
            GeometryManager::BindIndexBuffer();
            FlushLights(scene);
            for (RenderPass* rp : m_RenderPasses)
            {
                rp->Update(scene);
                rp->Bind();
                rp->Draw(scene);
                rp->Unbind();
            }
        }

        void Renderer::FlushLights(Scene* scene)
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

    }

}
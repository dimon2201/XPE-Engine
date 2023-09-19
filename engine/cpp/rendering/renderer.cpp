#include <rendering/renderer.h>
#include <rendering/monitor.h>
#include <rendering/draw/drawer.h>
#include <rendering/buffers/camera_buffer.h>
#include <rendering/buffers/light_buffers.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        Renderer::Renderer()
        {
            context::Init();
            ShaderManager::Init();
            Monitor::Init();

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

            Monitor::Free();
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
            FlushLights(scene);
            for (Drawer* drawer : m_Drawers) {
                drawer->Begin();
                drawer->Draw(scene);
                drawer->End();
            }
        }

        void Renderer::FlushLights(Scene* scene)
        {
            this->DirectLightBuffer->Clear();
            scene->EachComponent<DirectLightComponent>([this](DirectLightComponent* component)
            {
                this->DirectLightBuffer->Add(*component);
            });
            this->DirectLightBuffer->Flush();

            this->PointLightBuffer->Clear();
            scene->EachComponent<PointLightComponent>([this](PointLightComponent* component)
            {
                this->PointLightBuffer->Add(*component);
            });
            this->PointLightBuffer->Flush();

            this->SpotLightBuffer->Clear();
            scene->EachComponent<SpotLightComponent>([this](SpotLightComponent* component)
            {
                this->SpotLightBuffer->Add(*component);
            });
            this->SpotLightBuffer->Flush();
        }

    }

}
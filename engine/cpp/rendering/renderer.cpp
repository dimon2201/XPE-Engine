#include <rendering/renderer.h>
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

        void Renderer::Render(Scene* scene, RenderTarget* renderTarget)
        {
            FlushLights(scene);

            for (Drawer* drawer : m_Drawers) {
                drawer->Draw(scene, renderTarget);
            }

//            // Render 2D text
//            {
//                Text2DComponent text2D("Text2D");
//                stringstream ss;
//                ss << "CPU: " << CPUTime << "\n";
//                ss << "FPS: " << DeltaTime << "\n";
//                ss << "Current Time: " << CurrentTime << "\n";
//                text2D.Text = ss.str();
//
//                m_Text2DRenderer->Draw(text2D, m_Text2dTransform, m_Font);
//            }
//
//            // Render 3D text
//            {
//                Text3DComponent text3D("Text3D");
//                stringstream ss;
//                ss << "CPU: " << CPUTime << "\n";
//                ss << "FPS: " << DeltaTime << "\n";
//                ss << "Current Time: " << CurrentTime << "\n";
//                text3D.Text = ss.str();
//
//                m_Text3DRenderer->Draw(text3D, m_Text3dTransform, m_Font);
//            }
        }

        void Renderer::FlushLights(Scene* scene)
        {
            this->DirectLightBuffer->Clear();
            scene->EachComponent<DirectLightComponent>([this](DirectLightComponent* component) {
                this->DirectLightBuffer->AddComponent(*component);
            });
            this->DirectLightBuffer->Flush();

            this->PointLightBuffer->Clear();
            scene->EachComponent<PointLightComponent>([this](PointLightComponent* component) {
                this->PointLightBuffer->AddComponent(*component);
            });
            this->PointLightBuffer->Flush();

            this->SpotLightBuffer->Clear();
            scene->EachComponent<SpotLightComponent>([this](SpotLightComponent* component) {
                this->SpotLightBuffer->AddComponent(*component);
            });
            this->SpotLightBuffer->Flush();
        }

    }

}
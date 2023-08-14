#include <rendering/renderer.h>
#include <rendering/draw/drawer.h>
#include <rendering/buffers/camera_buffer.h>

#include <ecs/scene.h>

namespace xpe {

    namespace render {

        Renderer::Renderer()
        {
            context::Init();
            ShaderManager::Init();
            m_CameraBuffer = new CameraBuffer();
        }

        Renderer::~Renderer()
        {
            delete m_CameraBuffer;

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

    }

}
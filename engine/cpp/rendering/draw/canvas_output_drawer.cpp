#include <rendering/draw/canvas_output_drawer.h>
#include <rendering/storages/geometry_storage.h>
#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        CanvasOutputDrawer::CanvasOutputDrawer(
                CameraBuffer* cameraBuffer,
                Shader* shader,
                GeometryStorage* geometryStorage,
                Texture* colorTexture,
                Texture* aoTexture,
                RenderTarget* renderTarget
        ) : Drawer(cameraBuffer, shader, renderTarget)
        {
            m_Pipeline->InputLayout.Format = Vertex3D::Format;
            m_Pipeline->Shader = shader;
            m_Pipeline->Textures.emplace_back(colorTexture);
            m_Pipeline->Textures.emplace_back(aoTexture);
            m_Pipeline->RenderTarget = renderTarget;

            Init();
        }

        CanvasOutputDrawer::~CanvasOutputDrawer()
        {
        }

        void CanvasOutputDrawer::Draw(Scene* scene)
        {
            m_Pipeline->Textures[0]->Slot = 0;
            m_Pipeline->Textures[1]->Slot = 1;
            context::BindPrimitiveTopology(ePrimitiveTopology::TRIANGLE_STRIP);
            context::DrawVertexed(0, 4, 1);
        }

    }

}
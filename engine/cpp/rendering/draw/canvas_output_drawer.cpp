#include <rendering/draw/canvas_output_drawer.h>
#include <rendering/storages/geometry_storage.h>
#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        CanvasOutputDrawer::CanvasOutputDrawer(
                CameraBuffer* cameraBuffer,
                Shader* shader,
                GeometryStorage* geometryStorage,
                RenderTarget* renderTarget
        ) : Drawer(cameraBuffer, shader, renderTarget)
        {
            m_Quad = geometryStorage->GetGeometryIndexed2D("CanvasOutputDrawerQuad");
            m_Pipeline->InputLayout.Format = Vertex3D::Format;
            m_Pipeline->VertexBuffer = &m_Quad->Vertices;
            m_Pipeline->IndexBuffer = &m_Quad->Indices;
            m_Pipeline->Shader = shader;
            m_Pipeline->RenderTarget = renderTarget;

            Init();
        }

        CanvasOutputDrawer::~CanvasOutputDrawer() {}

        void CanvasOutputDrawer::Draw(Scene *scene)
        {
        }

    }

}
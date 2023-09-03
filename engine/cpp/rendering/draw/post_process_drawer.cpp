#include <rendering/draw/post_process_drawer.h>
#include <rendering/storages/geometry_storage.h>
#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        PostProcessDrawer::PostProcessDrawer(
            CameraBuffer* cameraBuffer,
            Shader* shader,
            GeometryStorage* geometryStorage,
            RenderTarget* renderTarget
        ) : Drawer(cameraBuffer, shader, renderTarget)
        {
            m_Quad = geometryStorage->GetGeometryIndexed2D("PostProcessQuad");
            m_Pipeline->InputLayout.Format = Vertex3D::Format;
            m_Pipeline->VertexBuffer = &m_Quad->Vertices;
            m_Pipeline->IndexBuffer = &m_Quad->Indices;
            m_Pipeline->Shader = shader;
            m_Pipeline->RenderTarget = renderTarget;

            Init();
        }

        PostProcessDrawer::~PostProcessDrawer()
        {
        }

        void PostProcessDrawer::Draw(Scene* scene)
        {
            context::BindPrimitiveTopology(m_Quad->PrimitiveTopology);
            context::DrawIndexed(0, 0, m_Quad->Indices.NumElements, 1);
        }

    }

}
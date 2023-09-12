#include <rendering/draw/ssao_drawer.h>
#include <rendering/storages/geometry_storage.h>
#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        SSAODrawer::SSAODrawer(
            CameraBuffer* cameraBuffer,
            Shader* shader,
            GeometryStorage* geometryStorage,
            Texture* positionTexture,
            Texture* depthTexture,
            RenderTarget* renderTarget
        ) : Drawer(cameraBuffer, shader, renderTarget)
        {
            context::CreateSampler(m_Sampler);
            m_Quad = geometryStorage->GetGeometryIndexed2D("SSAODrawerQuad");
            m_Pipeline->InputLayout.Format = Vertex3D::Format;
            m_Pipeline->VertexBuffer = &m_Quad->Vertices;
            m_Pipeline->IndexBuffer = &m_Quad->Indices;
            m_Pipeline->Shader = shader;
            m_Pipeline->Textures.emplace_back(positionTexture);
            m_Pipeline->Textures.emplace_back(depthTexture);
            m_Pipeline->Samplers.emplace_back(&m_Sampler);
            m_Pipeline->RenderTarget = renderTarget;

            Init();
        }

        SSAODrawer::~SSAODrawer()
        {
        }

        void SSAODrawer::Draw(Scene* scene)
        {
            m_Pipeline->Textures[0]->Slot = 0;
            m_Pipeline->Textures[1]->Slot = 1;
            context::BindPrimitiveTopology(m_Quad->PrimitiveTopology);
            context::DrawIndexed(0, 0, m_Quad->Indices.NumElements, 1);
        }

    }

}
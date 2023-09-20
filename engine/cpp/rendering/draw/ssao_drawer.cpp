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
            Texture* normalTexture,
            Texture* depthTexture,
            RenderTarget* renderTarget
        ) : Drawer(cameraBuffer, shader, renderTarget)
        {
            m_Buffer.Type = eBufferType::CONSTANT;
            m_Buffer.Usage = eBufferUsage::DYNAMIC;
            m_Buffer.NumElements = 1;
            m_Buffer.StructureSize = sizeof(SSAOBufferLayout);
            context::CreateBuffer(m_Buffer);

            m_Quad = geometryStorage->GetGeometryIndexed2D("SSAODrawerQuad");
            m_Pipeline->InputLayout.Format = Vertex3D::Format;
            m_Pipeline->VertexBuffer = &m_Quad->Vertices;
            m_Pipeline->IndexBuffer = &m_Quad->Indices;
            m_Pipeline->Shader = shader;
            m_Pipeline->VSBuffers.emplace_back(&m_Buffer);
            m_Pipeline->Textures.emplace_back(positionTexture);
            m_Pipeline->Textures.emplace_back(normalTexture);
            m_Pipeline->Textures.emplace_back(depthTexture);
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
            m_Pipeline->Textures[2]->Slot = 2;
            m_Pipeline->VSBuffers[0]->Slot = 1;

            m_BufferData.SSAODirectionCount = 4;
            m_BufferData.SSAOSampleCount = 4;
            void* memory = context::Map(m_Buffer, 0, eMapType::WRITE_DISCARD);
            memcpy(memory, &m_BufferData, sizeof(SSAOBufferLayout));
            context::Unmap(m_Buffer);

            context::BindPrimitiveTopology(m_Quad->PrimitiveTopology);
            context::DrawIndexed(0, 0, m_Quad->Indices.NumElements, 1);
        }

    }

}
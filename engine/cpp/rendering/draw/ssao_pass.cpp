#include <rendering/render_passes/ssao_pass.hpp>
#include <rendering/storages/geometry_storage.h>
#include <ecs/scene.h>

namespace xpe
{
    namespace render
    {
        SSAOPass::SSAOPass(GeometryStorage* geometry, const core::vector<RenderPassBinding>& bindings, RenderTarget* output)
            : RenderPass(bindings, output)
        {
            m_Quad = geometry->AddGeometryIndexed2D("SSAOQuad", Quad2D());

            m_Buffer.Type = eBufferType::CONSTANT;
            m_Buffer.Usage = eBufferUsage::DYNAMIC;
            m_Buffer.NumElements = 1;
            m_Buffer.StructureSize = sizeof(SSAOBufferLayout);
            m_Buffer.InitialData = nullptr;
            m_Buffer.Slot = 0;
            context::CreateBuffer(m_Buffer);

            m_Pipeline->VSBuffers.emplace_back(&m_Buffer);

            context::CreatePipeline(*m_Pipeline);
        }

        SSAOPass::~SSAOPass()
        {
        }

        void SSAOPass::Update(Scene* scene)
        {
            // TODO: very strange buffer update. In that case, better to replace CONSTANT buffer with STRUCTURED buffer.
            void* memory = context::Map(m_Buffer, 0, eMapType::WRITE_DISCARD);
            memcpy(memory, &m_BufferData, sizeof(SSAOBufferLayout));
            context::Unmap(m_Buffer);
        }

        void SSAOPass::Draw(Scene* scene)
        {
            context::BindPrimitiveTopology(m_Quad->PrimitiveTopology);
            context::DrawIndexed(0, 0, m_Quad->Indices.NumElements, 1);
        }
    }
}
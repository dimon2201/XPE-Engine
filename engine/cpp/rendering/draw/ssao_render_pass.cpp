#include "rendering/draw/ssao_render_pass.hpp"
#include "rendering/core/context.hpp"
#include "rendering/storages/geometry_storage.h"
#include "ecs/scene.h"

namespace xpe
{
    namespace render
    {
        SSAORenderPass::SSAORenderPass(GeometryStorage* geometry, const core::vector<RenderPassBinding>& bindings, RenderTarget* output)
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

        SSAORenderPass::~SSAORenderPass()
        {
        }

        void SSAORenderPass::Update(Scene* scene)
        {
            void* memory = context::Map(m_Buffer, 0, eMapType::WRITE_DISCARD);
            memcpy(memory, &m_BufferData, sizeof(SSAOBufferLayout));
            context::Unmap(m_Buffer);
        }

        void SSAORenderPass::Draw(Scene* scene)
        {
            context::BindPrimitiveTopology(m_Quad->PrimitiveTopology);
            context::DrawIndexed(0, 0, m_Quad->Indices.NumElements, 1);
        }
    }
}
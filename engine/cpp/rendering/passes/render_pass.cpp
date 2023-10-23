#include <rendering/passes/render_pass.h>
#include <rendering/core/context.hpp>
#include <rendering/core/pipeline.h>

namespace xpe {

    namespace render {

        RenderPass::RenderPass(const vector<RenderPassBinding>& bindings, RenderTarget* output)
        {
            m_Bindings = bindings;
            m_Pipeline = new Pipeline();

            for (auto& binding : m_Bindings)
            {
                switch (binding.Type) {

                    case RenderPassBinding::eType::BUFFER:
                        if (binding.Stage == RenderPassBinding::eStage::VERTEX)
                        {
                            m_Pipeline->VSBuffers.emplace_back((Buffer*)binding.Resource);
                            if (binding.Slot != RenderPassBinding::SLOT_DEFAULT) {
                                m_Pipeline->VSBuffers.back()->Slot = binding.Slot;
                            }
                        }
                        else if (binding.Stage == RenderPassBinding::eStage::PIXEL)
                        {
                            m_Pipeline->PSBuffers.emplace_back((Buffer*)binding.Resource);
                            if (binding.Slot != RenderPassBinding::SLOT_DEFAULT) {
                                m_Pipeline->PSBuffers.back()->Slot = binding.Slot;
                            }
                        }
                        break;

                    case RenderPassBinding::eType::TEXTURE:
                        if (binding.Stage == RenderPassBinding::eStage::PIXEL)
                        {
                            m_Pipeline->Textures.emplace_back((Texture*)binding.Resource);
                            if (binding.Slot != RenderPassBinding::SLOT_DEFAULT) {
                                m_Pipeline->Textures.back()->Slot = binding.Slot;
                            }
                        }
                        break;

                    case RenderPassBinding::eType::SAMPLER:
                        if (binding.Stage == RenderPassBinding::eStage::PIXEL)
                        {
                            m_Pipeline->Samplers.emplace_back((TextureSampler*)binding.Resource);
                            if (binding.Slot != RenderPassBinding::SLOT_DEFAULT) {
                                m_Pipeline->Samplers.back()->Slot = binding.Slot;
                            }
                        }
                        break;

                    case RenderPassBinding::eType::SHADER:
                        m_Pipeline->Shader = (Shader*)binding.Resource;
                        break;

                    case RenderPassBinding::eType::RASTERIZER:
                        m_Pipeline->Rasterizer = *((Rasterizer*) binding.Resource);
                        break;

                    case RenderPassBinding::eType::DEPTH_STENCIL:
                        m_Pipeline->DepthStencil = *((DepthStencilMode*) binding.Resource);
                        break;

                    case RenderPassBinding::eType::BLENDING:
                        m_Pipeline->Blending = *((BlendMode*) binding.Resource);
                        break;

                }
            }

            m_Pipeline->RenderTarget = output;
            m_Pipeline->InputLayout.Format = Vertex::Format;
        }

        RenderPass::~RenderPass()
        {
            context::FreePipeline(*m_Pipeline);
            delete m_Pipeline;
        }

        void RenderPass::Init()
        {
            context::CreatePipeline(*m_Pipeline);
        }

        void RenderPass::Bind()
        {
            context::BindPipeline(*m_Pipeline);
        }

        void RenderPass::Unbind()
        {
            context::UnbindPipeline(*m_Pipeline);
        }

    }

}
#include <rendering/draw/render_pass.h>
#include <rendering/buffers/camera_buffer.h>
#include <geometry/vertices.h>

namespace xpe {

    namespace render {

        RenderPass::RenderPass(const vector<RenderPassBinding>& bindings, RenderTarget* output)
        {
            m_Bindings = bindings;
            m_Pipeline = new Pipeline();

            for (auto& binding : m_Bindings)
            {
                if (binding.Type == RenderPassBinding::eType::BUFFER)
                {
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
                }
                else if (binding.Type == RenderPassBinding::eType::TEXTURE)
                {
                    if (binding.Stage == RenderPassBinding::eStage::PIXEL)
                    {
                        m_Pipeline->Textures.emplace_back((Texture*)binding.Resource);
                        if (binding.Slot != RenderPassBinding::SLOT_DEFAULT) {
                            m_Pipeline->Textures.back()->Slot = binding.Slot;
                        }
                    }
                }
                else if (binding.Type == RenderPassBinding::eType::SAMPLER)
                {
                    if (binding.Stage == RenderPassBinding::eStage::PIXEL)
                    {
                        m_Pipeline->Samplers.emplace_back((TextureSampler*)binding.Resource);
                        if (binding.Slot != RenderPassBinding::SLOT_DEFAULT) {
                            m_Pipeline->Samplers.back()->Slot = binding.Slot;
                        }
                    }
                }
                else if (binding.Type == RenderPassBinding::eType::SHADER)
                {
                    m_Pipeline->Shader = (Shader*)binding.Resource;
                }
            }

            m_Pipeline->InputLayout.Format = math::Vertex3D::Format;
            m_Pipeline->RenderTarget = output;
            m_Pipeline->Blending.Targets[0].Enable = true;
            m_Pipeline->Blending.Targets[0].Src = eBlend::SRC_ALPHA;
            m_Pipeline->Blending.Targets[0].Dest = eBlend::INV_SRC_ALPHA;
        }

        RenderPass::~RenderPass()
        {
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
#include <rendering/passes/render_pass.h>
#include <rendering/core/context.hpp>
#include <rendering/core/pipeline.h>

namespace xpe {

    namespace render {

        RenderPass::~RenderPass()
        {
            context::FreePipeline(*m_Pipeline);
            delete m_Pipeline;
        }

        RenderPass::RenderPass(eType type, const vector<RenderPassBinding>& bindings)
        {
            m_Type = type;
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

                    case RenderPassBinding::eType::RENDER_TARGET:
                        m_Pipeline->RenderTarget = ((RenderTarget*) binding.Resource);
                        break;

                }
            }

            m_Pipeline->InputLayout.Format = Vertex::Format;
        }

        void RenderPass::InitOpaque()
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ALL;

            BlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.IndependentBlendEnable = true;

            m_Pipeline->Rasterizer.CullMode = eCullMode::NONE;
        }

        void RenderPass::InitTransparent()
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ZERO;

            BlendTarget target;
            target.Enable = true;
            target.Src = eBlend::ONE;
            target.Dest = eBlend::ONE;
            target.BlendOp = eBlendOp::ADD;
            target.SrcAlpha = eBlend::ONE;
            target.DestAlpha = eBlend::ONE;
            target.BlendOpAlpha = eBlendOp::ADD;
            m_Pipeline->Blending.Targets.push_back(target);

            target.Enable = true;
            target.Src = eBlend::ZERO;
            target.Dest = eBlend::INV_SRC_COLOR;
            target.BlendOp = eBlendOp::ADD;
            target.SrcAlpha = eBlend::ZERO;
            target.DestAlpha = eBlend::ZERO;
            target.BlendOpAlpha = eBlendOp::ADD;
            m_Pipeline->Blending.Targets.push_back(target);

            m_Pipeline->Blending.IndependentBlendEnable = true;

            m_Pipeline->Rasterizer.CullMode = eCullMode::NONE;
        }

        void RenderPass::InitPostFX()
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ALL;
            BlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.IndependentBlendEnable = true;
        }

        void RenderPass::InitShadow()
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ALL;
            m_Pipeline->Rasterizer.CullMode = eCullMode::FRONT;
        }

        void RenderPass::Init()
        {
            switch (m_Type)
            {
                case RenderPass::eType::OPAQUE:
                    InitOpaque();
                    break;

                case RenderPass::eType::TRANSPARENT:
                    InitTransparent();
                    break;

                case RenderPass::eType::POSTFX:
                    InitPostFX();
                    break;

                case RenderPass::eType::SHADOW:
                    InitShadow();
                    break;
            }

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

        RenderTarget* RenderPass::GetRenderTarget()
        {
            return m_Pipeline->RenderTarget;
        }

        void RenderPass::DrawPostFX(Scene* scene)
        {
            context::DrawQuad();
        }

    }

}
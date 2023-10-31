#include <rendering/passes/render_pass.h>
#include <rendering/core/context.hpp>
#include <rendering/core/pipeline.h>

namespace xpe {

    namespace render {

        cRenderPass::~cRenderPass()
        {
            context::FreePipeline(*m_Pipeline);
            delete m_Pipeline;
        }

        cRenderPass::cRenderPass(eType type, const vector<sRenderPassBinding>& bindings)
        {
            m_Type = type;
            m_Bindings = bindings;
            m_Pipeline = new sPipeline();
            m_Pipeline->InputLayout.Format = sVertex::k_Format;

            for (auto& binding : m_Bindings)
            {
                switch (binding.Type) {

                    case sRenderPassBinding::eType::BUFFER:
                        if (binding.Stage == sRenderPassBinding::eStage::VERTEX)
                        {
                            m_Pipeline->VSBuffers.emplace_back((sBuffer*)binding.Resource);
                            if (binding.Slot != K_SLOT_DEFAULT) {
                                m_Pipeline->VSBuffers.back()->Slot = binding.Slot;
                            }
                        }
                        else if (binding.Stage == sRenderPassBinding::eStage::PIXEL)
                        {
                            m_Pipeline->PSBuffers.emplace_back((sBuffer*)binding.Resource);
                            if (binding.Slot != K_SLOT_DEFAULT) {
                                m_Pipeline->PSBuffers.back()->Slot = binding.Slot;
                            }
                        }
                        break;

                    case sRenderPassBinding::eType::TEXTURE:
                        if (binding.Stage == sRenderPassBinding::eStage::PIXEL)
                        {
                            m_Pipeline->Textures.emplace_back((sTexture*)binding.Resource);
                            if (binding.Slot != K_SLOT_DEFAULT) {
                                m_Pipeline->Textures.back()->Slot = binding.Slot;
                            }
                        }
                        break;

                    case sRenderPassBinding::eType::SAMPLER:
                        if (binding.Stage == sRenderPassBinding::eStage::PIXEL)
                        {
                            m_Pipeline->Samplers.emplace_back((sSampler*)binding.Resource);
                            if (binding.Slot != K_SLOT_DEFAULT) {
                                m_Pipeline->Samplers.back()->Slot = binding.Slot;
                            }
                        }
                        break;

                    case sRenderPassBinding::eType::SHADER:
                        m_Pipeline->Shader = (sShader*)binding.Resource;
                        break;

                    case sRenderPassBinding::eType::RASTERIZER:
                        m_Pipeline->Rasterizer = *((sRasterizer*) binding.Resource);
                        break;

                    case sRenderPassBinding::eType::DEPTH_STENCIL:
                        m_Pipeline->DepthStencil = *((sDepthStencilMode*) binding.Resource);
                        break;

                    case sRenderPassBinding::eType::BLENDING:
                        m_Pipeline->Blending = *((sBlendMode*) binding.Resource);
                        break;

                    case sRenderPassBinding::eType::RENDER_TARGET:
                        m_Pipeline->RenderTarget = ((sRenderTarget*) binding.Resource);
                        break;

                }
            }
        }

        void cRenderPass::InitFinal()
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ALL;
            sBlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.IndependentBlendEnable = true;
        }

        void cRenderPass::InitShadow()
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ALL;
            m_Pipeline->Rasterizer.CullMode = eCullMode::FRONT;
        }

        void cRenderPass::InitOpaque()
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ALL;

            sBlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.IndependentBlendEnable = true;

            m_Pipeline->Rasterizer.CullMode = eCullMode::NONE;
        }

        void cRenderPass::InitTransparent()
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ZERO;

            sBlendTarget target;
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

        void cRenderPass::InitPostFX()
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ALL;

            sBlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.IndependentBlendEnable = true;
        }

        void cRenderPass::InitUI()
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ALL;
            sBlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.IndependentBlendEnable = true;
        }

        void cRenderPass::Init()
        {
            switch (m_Type)
            {
                case cRenderPass::eType::FINAL:
                    InitFinal();
                    break;

                case cRenderPass::eType::SHADOW:
                    InitShadow();
                    break;

                case cRenderPass::eType::OPAQUE:
                    InitOpaque();
                    break;

                case cRenderPass::eType::TRANSPARENT:
                    InitTransparent();
                    break;

                case cRenderPass::eType::POSTFX:
                    InitPostFX();
                    break;

                case cRenderPass::eType::UI:
                    InitUI();
                    break;
            }

            context::CreatePipeline(*m_Pipeline);
        }

        void cRenderPass::Bind()
        {
            context::BindPipeline(*m_Pipeline);
        }

        void cRenderPass::Unbind()
        {
            context::UnbindPipeline(*m_Pipeline);
        }

        sRenderTarget* cRenderPass::GetRenderTarget()
        {
            return m_Pipeline->RenderTarget;
        }

        void cRenderPass::DrawFinal(cScene* scene)
        {
            context::DrawQuad();
        }

        void cRenderPass::DrawPostFX(cScene* scene)
        {
            context::DrawQuad();
        }

    }

}
#include <rendering/passes/render_pass.h>
#include <rendering/core/context.hpp>
#include <rendering/core/pipeline.h>

namespace xpe {

    namespace render {

        cRenderPass::~cRenderPass()
        {
            context::FreeVertexPipeline(*m_Pipeline);
            delete m_Pipeline;
        }

        cRenderPass::cRenderPass(eType type, const vector<sRenderPassBinding>& bindings) : m_Type(type), m_Bindings(bindings)
        {
            m_Pipeline = new sVertexPipeline();
            m_Pipeline->InputLayout.Format = sVertex::k_Format;

            for (auto& binding : m_Bindings)
            {
                switch (binding.Type) {

                    case sRenderPassBinding::eType::BUFFER:
                        for (auto* stage : m_Pipeline->Shader->Stages)
                        {
                            if (stage->Type == binding.Stage) {
                                stage->Buffers.emplace_back((sBuffer*)binding.Resource);
                                if (binding.Slot != K_SLOT_DEFAULT) {
                                    stage->Buffers.back()->Slot = binding.Slot;
                                }
                            }
                        }
                        break;

                    case sRenderPassBinding::eType::TEXTURE:
                        for (auto* stage : m_Pipeline->Shader->Stages)
                        {
                            if (stage->Type == binding.Stage) {
                                stage->Textures.emplace_back((sTexture*)binding.Resource);
                                if (binding.Slot != K_SLOT_DEFAULT) {
                                    stage->Textures.back()->Slot = binding.Slot;
                                }
                            }
                        }
                        break;

                    case sRenderPassBinding::eType::SAMPLER:
                        for (auto* stage : m_Pipeline->Shader->Stages)
                        {
                            if (stage->Type == binding.Stage) {
                                stage->Samplers.emplace_back((sSampler*)binding.Resource);
                                if (binding.Slot != K_SLOT_DEFAULT) {
                                    stage->Samplers.back()->Slot = binding.Slot;
                                }
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

            sBlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.IndependentBlendEnable = true;

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
            target.Enable = true;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.Targets.push_back(target);
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

            context::CreateVertexPipeline(*m_Pipeline);
        }

        void cRenderPass::Bind()
        {
            context::BindVertexPipeline(*m_Pipeline);
        }

        void cRenderPass::Unbind()
        {
            context::UnbindVertexPipeline(*m_Pipeline);
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

        cComputePass::~cComputePass()
        {
            context::FreeComputePipeline(*m_Pipeline);
            delete m_Pipeline;
        }

        cComputePass::cComputePass(const vector<sComputePassBinding> &bindings, const glm::ivec3& threadGroups)
        : m_Bindings(bindings), m_ThreadGroups(threadGroups)
        {
            m_Pipeline = new sComputePipeline();

            for (auto& binding : m_Bindings)
            {
                switch (binding.Type) {

                    case sComputePassBinding::eType::BUFFER:
                        for (auto* stage : m_Pipeline->Shader->Stages)
                        {
                            if (stage->Type == sShaderStage::eType::COMPUTE)
                            {
                                stage->Buffers.emplace_back((sBuffer*)binding.Resource);
                                if (binding.Slot != K_SLOT_DEFAULT) {
                                    stage->Buffers.back()->Slot = binding.Slot;
                                }
                            }
                        }
                        break;

                    case sComputePassBinding::eType::TEXTURE:
                        for (auto* stage : m_Pipeline->Shader->Stages)
                        {
                            if (stage->Type == sShaderStage::eType::COMPUTE)
                            {
                                stage->Textures.emplace_back((sTexture*)binding.Resource);
                                if (binding.Slot != K_SLOT_DEFAULT) {
                                    stage->Textures.back()->Slot = binding.Slot;
                                }
                            }
                        }
                        break;

                    case sComputePassBinding::eType::SAMPLER:
                        for (auto* stage : m_Pipeline->Shader->Stages)
                        {
                            if (stage->Type == sShaderStage::eType::COMPUTE)
                            {
                                stage->Samplers.emplace_back((sSampler*)binding.Resource);
                                if (binding.Slot != K_SLOT_DEFAULT) {
                                    stage->Samplers.back()->Slot = binding.Slot;
                                }
                            }
                        }
                        break;

                    case sComputePassBinding::eType::SHADER:
                        m_Pipeline->Shader = (sShader*)binding.Resource;
                        break;

                    case sComputePassBinding::eType::RENDER_TARGET:
                        m_Pipeline->RenderTarget = ((sRenderTarget*) binding.Resource);
                        break;

                }
            }
        }

        void cComputePass::Init()
        {
            context::CreateComputePipeline(*m_Pipeline);
        }

        void cComputePass::Bind()
        {
            context::BindComputePipeline(*m_Pipeline);
        }

        void cComputePass::Unbind()
        {
            context::UnbindComputePipeline(*m_Pipeline);
        }

        void cComputePass::Dispatch()
        {
            context::Dispatch(m_ThreadGroups);
        }

        sRenderTarget* cComputePass::GetRenderTarget()
        {
            return m_Pipeline->RenderTarget;
        }

    }

}
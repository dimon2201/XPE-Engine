#include <rendering/passes/composite_transparent_pass.h>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        CompositeTransparentPass::CompositeTransparentPass(const vector<RenderPassBinding>& bindings)
        : RenderPass(eType::POSTFX, bindings) {}

        void CompositeTransparentPass::InitPostFX()
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ZERO;

            BlendTarget target;
            target.Enable = true;
            target.Src = eBlend::SRC_ALPHA;
            target.Dest = eBlend::INV_SRC_ALPHA;
            target.BlendOp = eBlendOp::ADD;
            target.SrcAlpha = eBlend::SRC_ALPHA;
            target.DestAlpha = eBlend::INV_SRC_ALPHA;
            target.BlendOpAlpha = eBlendOp::ADD;
            m_Pipeline->Blending.Targets.push_back(target);
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.IndependentBlendEnable = true;
        }

    }

}
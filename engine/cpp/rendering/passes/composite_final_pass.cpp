#include <rendering/passes/composite_final_pass.h>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        CompositeFinalPass::CompositeFinalPass(const vector<RenderPassBinding>& bindings)
        : RenderPass(eType::POSTFX, bindings)
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ALL;

            BlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.IndependentBlendEnable = true;
        }

        void CompositeFinalPass::Draw(Scene* scene)
        {
            context::DrawQuad();
        }

    }

}
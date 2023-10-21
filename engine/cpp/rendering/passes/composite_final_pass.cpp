#include <rendering/passes/composite_final_pass.h>
#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        CompositeFinalPass::CompositeFinalPass(
            const core::vector<RenderPassBinding>& bindings
        ) : RenderPass(bindings)
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ALL;

            BlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);

            context::CreatePipeline(*m_Pipeline);
        }

        CompositeFinalPass::~CompositeFinalPass()
        {
        }

        void CompositeFinalPass::Draw(Scene* scene)
        {
            context::DrawQuad();
        }

    }

}
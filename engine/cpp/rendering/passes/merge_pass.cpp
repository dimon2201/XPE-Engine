#include <rendering/passes/merge_pass.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        MergePass::MergePass(
                const vector<RenderPassBinding>& bindings,
                RenderTarget* output
        ) : RenderPass(bindings, output)
        {
            context::CreatePipeline(*m_Pipeline);
        }

        MergePass::~MergePass()
        {
        }

        void MergePass::Update(Scene* scene)
        {
        }

        void MergePass::Draw(Scene* scene)
        {
            context::BindPrimitiveTopology(ePrimitiveTopology::TRIANGLE_STRIP);
            context::DrawVertexed(4);
        }

    }

}
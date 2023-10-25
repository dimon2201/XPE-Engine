#include <rendering/passes/merge_pass.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        void MergePass::Draw(Scene* scene)
        {
            context::BindPrimitiveTopology(ePrimitiveTopology::TRIANGLE_STRIP);
            context::DrawVertexed(4);
        }

    }

}
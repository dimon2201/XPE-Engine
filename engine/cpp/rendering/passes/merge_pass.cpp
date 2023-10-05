#include <rendering/passes/merge_pass.h>
#include <rendering/storages/geometry_storage.h>
#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        MergePass::MergePass(
            const core::vector<RenderPassBinding>& bindings,
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
            context::DrawQuad();
        }

    }

}
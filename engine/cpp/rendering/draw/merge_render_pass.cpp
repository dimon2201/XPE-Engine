#include <rendering/draw/merge_render_pass.h>
#include <rendering/storages/geometry_storage.h>
#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        MergeRenderPass::MergeRenderPass(
            const core::vector<RenderPassBinding>& bindings,
            RenderTarget* output,
            GeometryStorage* geometryStorage
        ) : RenderPass(bindings, output)
        {
            context::CreatePipeline(*m_Pipeline);
        }

        MergeRenderPass::~MergeRenderPass()
        {
        }

        void MergeRenderPass::Update(Scene* scene)
        {
        }

        void MergeRenderPass::Draw(Scene* scene)
        {
            context::BindPrimitiveTopology(ePrimitiveTopology::TRIANGLE_STRIP);
            context::DrawVertexed(0, 4, 1);
        }

    }

}
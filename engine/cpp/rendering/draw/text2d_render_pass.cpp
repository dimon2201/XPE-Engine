#include <rendering/draw/text2d_render_pass.h>
#include <rendering/buffers/viewport_buffer.h>
#include <rendering/storages/geometry_storage.h>
#include <rendering/font/font.hpp>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        Text2DRenderPass::Text2DRenderPass(
            const core::vector<RenderPassBinding>& bindings,
            RenderTarget* output,
            GeometryStorage* geometryStorage
        ) : TextRenderPass(bindings, output, geometryStorage)
        {
            context::CreatePipeline(*m_Pipeline);
        }

        Text2DRenderPass::~Text2DRenderPass() {}

        void Text2DRenderPass::Update(Scene* scene)
        {
        }

        void Text2DRenderPass::Draw(Scene *scene)
        {
            scene->EachComponent<Text2DComponent>([this](Text2DComponent* component)
            {
                DrawText(component->Transform, component->Text, component->Font);
            });
        }

    }

}
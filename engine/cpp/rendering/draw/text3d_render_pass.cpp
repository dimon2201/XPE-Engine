#include <rendering/draw/text3d_render_pass.h>
#include <rendering/storages/geometry_storage.h>
#include <rendering/font/font.hpp>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        Text3DRenderPass::Text3DRenderPass(
            const core::vector<RenderPassBinding>& bindings,
            RenderTarget* output,
            GeometryStorage* geometryStorage
        ) : TextRenderPass(bindings, output, geometryStorage)
        {
        }

        Text3DRenderPass::~Text3DRenderPass() {}

        void Text3DRenderPass::Update(Scene* scene)
        {
        }

        void Text3DRenderPass::Draw(Scene* scene)
        {
            scene->EachComponent<Text3DComponent>([this](Text3DComponent* component)
            {
                DrawText(component->Transform, component->Text, component->Font);
            });
        }

    }

}
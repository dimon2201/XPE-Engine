#include <rendering/render_passes/text3d_pass.h>
#include <rendering/storages/geometry_storage.h>
#include <rendering/font/font.hpp>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        Text3DPass::Text3DPass(
            const core::vector<RenderPassBinding>& bindings,
            RenderTarget* output,
            GeometryStorage* geometryStorage
        ) : TextPass(bindings, output, geometryStorage)
        {
        }

        Text3DPass::~Text3DPass() {}

        void Text3DPass::Update(Scene* scene)
        {
        }

        void Text3DPass::Draw(Scene* scene)
        {
            scene->EachComponent<Text3DComponent>([this](Text3DComponent* component)
            {
                DrawText(component->Transform, component->Text, component->Font);
            });
        }

    }

}
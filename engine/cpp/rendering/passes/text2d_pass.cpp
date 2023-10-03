#include <rendering/passes/text2d_pass.h>
#include <rendering/storages/geometry_storage.h>
#include <rendering/font/font.hpp>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        Text2DPass::Text2DPass(
            const core::vector<RenderPassBinding>& bindings,
            GeometryStorage* geometryStorage,
            RenderTarget* output
        ) : TextPass(bindings, geometryStorage, output)
        {
            context::CreatePipeline(*m_Pipeline);
        }

        Text2DPass::~Text2DPass() {}

        void Text2DPass::Update(Scene* scene)
        {
        }

        void Text2DPass::Draw(Scene *scene)
        {
            scene->EachComponent<Text2DComponent>([this](Text2DComponent* component)
            {
                DrawText(component->Transform, component->Text, component->Font);
            });
        }

    }

}
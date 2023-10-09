#include <rendering/passes/text2d_pass.h>
#include <rendering/font/font.hpp>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        Text2DPass::Text2DPass(
                const vector<RenderPassBinding>& bindings,
                RenderTarget* output
        ) : TextPass(bindings, output)
        {
            context::CreatePipeline(*m_Pipeline);
        }

        Text2DPass::~Text2DPass() {}

        void Text2DPass::Draw(Scene* scene)
        {
            scene->EachComponent<Text2DComponent>([this](Text2DComponent* component)
            {
                DrawText(component->Entity->Transform, component->Text, component->Font);
            });
        }

    }

}
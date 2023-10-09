#include <rendering/passes/text3d_pass.h>
#include <rendering/font/font.hpp>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        Text3DPass::Text3DPass(
                const vector<RenderPassBinding>& bindings,
                RenderTarget* output
        ) : TextPass(bindings, output)
        {
            context::CreatePipeline(*m_Pipeline);
        }

        Text3DPass::~Text3DPass() {}

        void Text3DPass::Draw(Scene* scene)
        {
            scene->EachComponent<Text3DComponent>([this](Text3DComponent* component)
            {
                DrawText(component->Entity->Transform, component->Text, component->Font);
            });
        }

    }

}
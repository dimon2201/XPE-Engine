#include <rendering/passes/text2d_pass.h>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        Text2DPass::Text2DPass(const vector<RenderPassBinding> &bindings)
        : TextPass(eType::UI, bindings) {}

        void Text2DPass::DrawUI(Scene* scene)
        {
            scene->EachComponent<Text2DComponent>([this](Text2DComponent* component)
            {
                DrawText(component->Entity->Transform, component->Text, *component->Font);
            });
        }

    }

}
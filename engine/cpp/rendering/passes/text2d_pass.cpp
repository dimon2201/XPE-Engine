#include <rendering/passes/text2d_pass.h>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        Text2DPass::Text2DPass(const vector<RenderPassBinding> &bindings)
        : TextPass(eType::UI, bindings) {}

        void Text2DPass::DrawUI(cScene* scene)
        {
            scene->EachComponent<sCText2D>([this](sCText2D* component)
            {
                DrawText(component->Entity->Transform, component->Text, *component->Font);
            });
        }

    }

}
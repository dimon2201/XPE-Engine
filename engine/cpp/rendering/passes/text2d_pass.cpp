#include <rendering/passes/text2d_pass.h>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        cText2DPass::cText2DPass(const vector<sRenderPassBinding> &bindings)
        : cTextPass(eType::UI, bindings) {}

        void cText2DPass::DrawUI(cScene* scene)
        {
            scene->EachComponent<sCText2D>([this](sCText2D* component)
            {
                DrawText(component->Entity->GetTransform(), component->Text, *component->Font);
            });
        }

    }

}
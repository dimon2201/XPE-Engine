#include <rendering/passes/text3d_pass.h>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        cText3DPass::cText3DPass(const vector<sRenderPassBinding> &bindings)
        : cTextPass(eType::OPAQUE, bindings) {}

        void cText3DPass::DrawOpaque(cScene* scene)
        {
            scene->EachComponent<sCText3D>([this](sCText3D* component)
            {
                DrawText(component->Entity->Transform, component->Text, *component->Font);
            });
        }

    }

}
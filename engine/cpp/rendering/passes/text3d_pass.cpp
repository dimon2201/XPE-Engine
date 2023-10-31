#include <rendering/passes/text3d_pass.h>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        Text3DPass::Text3DPass(const vector<RenderPassBinding> &bindings)
        : TextPass(eType::OPAQUE, bindings) {}

        void Text3DPass::DrawOpaque(cScene* scene)
        {
            scene->EachComponent<sCText3D>([this](sCText3D* component)
            {
                DrawText(component->Entity->Transform, component->Text, *component->Font);
            });
        }

    }

}
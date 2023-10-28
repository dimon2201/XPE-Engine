#include <rendering/passes/text3d_pass.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        Text3DPass::Text3DPass(const vector<RenderPassBinding> &bindings)
        : TextPass(eType::OPAQUE, bindings) {}

        void Text3DPass::DrawOpaque(Scene* scene)
        {
            scene->EachComponent<Text3DComponent>([this](Text3DComponent* component)
            {
                DrawText(component->Entity->Transform, component->Text, component->Font);
            });
        }

    }

}
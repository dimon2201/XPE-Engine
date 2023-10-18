#include <rendering/passes/text3d_pass.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        void Text3DPass::Draw(Scene* scene)
        {
            scene->EachComponent<Text3DComponent>([this](Text3DComponent* component)
            {
                DrawText(component->Entity->Transform, component->Text, component->Font);
            });
        }

    }

}
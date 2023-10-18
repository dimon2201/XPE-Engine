#include <rendering/passes/text2d_pass.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        void Text2DPass::Draw(Scene* scene)
        {
            scene->EachComponent<Text2DComponent>([this](Text2DComponent* component)
            {
                DrawText(component->Entity->Transform, component->Text, component->Font);
            });
        }

    }

}
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
                sTransform transform = component->Entity->Transform;
                transform.Position = {
                        transform.Position.x * (float) cWindowManager::GetFrameWidth(),
                        transform.Position.y * (float) cWindowManager::GetFrameHeight() - component->Font->GlyphSize,
                        transform.Position.z
                };
                DrawText(transform, component->Text, *component->Font);
            });
        }

    }

}
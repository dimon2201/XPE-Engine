#pragma once

#include <rendering/passes/render_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API ShadowPass : public RenderPass {

        public:
            ShadowPass(const vector<RenderPassBinding>& bindings) : RenderPass(bindings) {}

            void Draw(Scene* scene) override final;

        };

    }

}
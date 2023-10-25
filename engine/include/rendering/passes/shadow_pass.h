#pragma once

#include <rendering/passes/instancing_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API ShadowPass : public InstancingPass {

        public:
            ShadowPass(eType type, const vector<RenderPassBinding>& bindings) : InstancingPass(type, bindings) {}

            void Draw(Scene* scene) override final;

        };

    }

}
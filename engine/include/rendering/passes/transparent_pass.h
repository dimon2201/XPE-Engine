#pragma once

#include <rendering/passes/instancing_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API TransparentPass : public InstancingPass
        {

        public:
            TransparentPass(
                    const vector<RenderPassBinding>& bindings,
                    MaterialStorage* materialStorage,
                    RenderTarget* output
            );

            virtual void Draw(Scene* scene) override;

        };

    }

}
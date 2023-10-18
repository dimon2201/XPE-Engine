#pragma once

#include <rendering/passes/instancing_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API MainPass : public InstancingPass
        {

        public:
            MainPass(
                    const vector<RenderPassBinding>& bindings,
                    RenderTarget* output,
                    MaterialStorage* materialStorage
            );

            virtual void Draw(Scene* scene) override;

        };

    }

}
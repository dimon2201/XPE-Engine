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

            ~MainPass() override;

            virtual void Update(Scene* scene) override;
            virtual void Draw(Scene* scene) override;

        };

    }

}
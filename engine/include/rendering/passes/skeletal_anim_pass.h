#pragma once

#include <rendering/passes/instancing_pass.h>

namespace xpe {

    namespace render {

        class SkeletalAnimPass : public InstancingPass
        {

        public:
            SkeletalAnimPass(
                const vector<RenderPassBinding>& bindings,
                RenderTarget* output
            );

            virtual void Draw(Scene* scene) override final;

        };

    }

}
#pragma once

#include <rendering/passes/instancing_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API OpaquePass : public InstancingPass
        {

        public:
            OpaquePass(const vector<RenderPassBinding>& bindings);

            virtual void Draw(Scene* scene) override;

        };

    }

}
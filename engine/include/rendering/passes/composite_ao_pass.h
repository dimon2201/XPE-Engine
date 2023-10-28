#pragma once

#include <rendering/passes/render_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API CompositeAOPass : public RenderPass
        {

        public:
            CompositeAOPass(const vector<RenderPassBinding>& bindings);

        };

    }

}
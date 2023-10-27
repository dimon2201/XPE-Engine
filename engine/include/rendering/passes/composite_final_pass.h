#pragma once

#include <rendering/passes/render_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API CompositeFinalPass : public RenderPass
        {

        public:
            CompositeFinalPass(const vector<RenderPassBinding>& bindings);

        };

    }

}
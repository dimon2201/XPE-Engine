#pragma once

#include <rendering/passes/render_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API FinalPass : public RenderPass
        {

        public:
            FinalPass(const vector<RenderPassBinding>& bindings);

        };

    }

}
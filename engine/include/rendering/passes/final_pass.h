#pragma once

#include <rendering/passes/render_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API cFinalPass : public cRenderPass
        {

        public:
            cFinalPass(const vector<sRenderPassBinding>& bindings);

        };

    }

}
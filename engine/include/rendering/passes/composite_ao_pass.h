#pragma once

#include <rendering/passes/render_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API cCompositeAoPass : public cRenderPass
        {

        public:
            cCompositeAoPass(const vector<sRenderPassBinding>& bindings);

        };

    }

}
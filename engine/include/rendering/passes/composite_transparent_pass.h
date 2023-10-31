#pragma once

#include <rendering/passes/render_pass.h>

namespace xpe {

    namespace render {

        using namespace ecs;

        class ENGINE_API cCompositeTransparentPass : public cRenderPass
        {

        public:
            cCompositeTransparentPass(const vector<sRenderPassBinding>& bindings);

        protected:
            void InitPostFX() override;

        };

    }

}
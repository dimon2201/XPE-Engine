#pragma once

#include <rendering/passes/render_pass.h>

namespace xpe {

    namespace render {

        using namespace ecs;

        class ENGINE_API CompositeTransparentPass : public RenderPass
        {

        public:
            CompositeTransparentPass(const vector<RenderPassBinding>& bindings);

            void Draw(Scene* scene) override;

        };

    }

}
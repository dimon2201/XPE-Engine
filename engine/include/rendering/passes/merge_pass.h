#pragma once

#include <rendering/passes/render_pass.h>

namespace xpe {

    namespace render {

        using namespace ecs;

        class ENGINE_API MergePass : public RenderPass
        {

        public:
            MergePass(
                    const vector<RenderPassBinding>& bindings
            ) : RenderPass(bindings) {}

            virtual void Draw(Scene* scene) override;

        };

    }

}
#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/geometry/geometries.h>

namespace xpe {

    namespace render {

        class ENGINE_API CompositeFinalPass : public RenderPass
        {

        public:
            CompositeFinalPass(const vector<RenderPassBinding>& bindings);

            virtual void Draw(Scene* scene) override;

        };

    }

}
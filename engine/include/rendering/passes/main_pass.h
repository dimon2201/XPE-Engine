#pragma once

#include <rendering/passes/instancing_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API MainPass : public InstancingPass
        {

        public:
            MainPass(eType type, const vector<RenderPassBinding>& bindings);

            void DrawOpaque(Scene *scene) override;
            void DrawTransparent(Scene *scene) override;

        };

    }

}
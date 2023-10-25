#pragma once

#include <rendering/passes/instancing_pass.h>

namespace xpe {

    namespace render {

        class SkeletalAnimPass : public InstancingPass
        {

        public:
            SkeletalAnimPass(eType type, const vector<RenderPassBinding>& bindings);

            void DrawOpaque(Scene *scene) override;
            void DrawTransparent(Scene *scene) override;

        };

    }

}
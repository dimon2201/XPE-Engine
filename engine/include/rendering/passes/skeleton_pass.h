#pragma once

#include <rendering/passes/instancing_pass.h>

namespace xpe {

    namespace render {

        class SkeletonPass : public InstancingPass
        {

        public:
            SkeletonPass(eType type, const vector<RenderPassBinding>& bindings);

            void DrawOpaque(Scene *scene) override;
            void DrawTransparent(Scene *scene) override;

            void DrawShadow(Scene *scene) override;

        protected:
            void InitOpaque() override;
            void InitTransparent() override;

        };

    }

}
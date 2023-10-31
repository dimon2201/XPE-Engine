#pragma once

#include <rendering/passes/instancing_pass.h>

namespace xpe {

    namespace render {

        class cSkeletonPass : public cInstancingPass
        {

        public:
            cSkeletonPass(eType type, const vector<sRenderPassBinding>& bindings);

            void DrawOpaque(cScene *scene) override;
            void DrawTransparent(cScene *scene) override;
            void DrawShadow(cScene *scene) override;

        protected:
            void InitOpaque() override;
            void InitTransparent() override;

        };

    }

}
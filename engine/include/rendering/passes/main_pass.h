#pragma once

#include <rendering/passes/instancing_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API cMainPass : public cInstancingPass
        {

        public:
            cMainPass(eType type, const vector<sRenderPassBinding>& bindings);

            void DrawOpaque(cScene *scene) override;
            void DrawTransparent(cScene *scene) override;
            void DrawShadow(cScene *scene) override;

        protected:
            void InitOpaque() override;
            void InitTransparent() override;

        };

    }

}
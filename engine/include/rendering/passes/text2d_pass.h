#pragma once

#include <rendering/passes/text_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API cText2DPass : public cTextPass
        {
        public:
            cText2DPass(const vector<sRenderPassBinding>& bindings);
            void DrawUI(cScene* scene) override;
        };

    }

}
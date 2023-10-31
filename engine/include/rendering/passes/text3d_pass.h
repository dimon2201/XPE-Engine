#pragma once

#include <rendering/passes/text_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API cText3DPass : public cTextPass
        {
        public:
            cText3DPass(const vector<sRenderPassBinding>& bindings);
            void DrawOpaque(cScene* scene) override;
        };

    }

}
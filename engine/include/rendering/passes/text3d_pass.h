#pragma once

#include <rendering/passes/text_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API Text3DPass : public TextPass
        {
        public:
            Text3DPass(const vector<RenderPassBinding>& bindings);
            void DrawOpaque(cScene* scene) override;
        };

    }

}
#pragma once

#include <rendering/passes/text_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API Text3DPass : public TextPass
        {

        public:
            Text3DPass(eType type, const vector<RenderPassBinding>& bindings) : TextPass(type, bindings) {}

            void Draw(Scene* scene) override;

        };

    }

}
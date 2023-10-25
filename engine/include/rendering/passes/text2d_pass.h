#pragma once

#include <rendering/passes/text_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API Text2DPass : public TextPass
        {

        public:
            Text2DPass(eType type, const vector<RenderPassBinding>& bindings) : TextPass(type, bindings) {}

            void Draw(Scene *scene) override;

        };

    }

}
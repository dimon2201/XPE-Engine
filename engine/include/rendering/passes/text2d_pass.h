#pragma once

#include <rendering/passes/text_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API Text2DPass : public TextPass
        {

        public:
            Text2DPass(
                    const vector<RenderPassBinding>& bindings,
                    RenderTarget* output
            ) : TextPass(bindings, output) {}

            virtual void Draw(Scene *scene) override;

        };

    }

}
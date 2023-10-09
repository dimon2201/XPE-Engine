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
            );
            ~Text2DPass() override;

            virtual void Draw(Scene *scene) override;

        };

    }

}
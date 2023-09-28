#pragma once

#include <rendering/render_passes/text_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API Text2DPass : public TextPass
        {

        public:
            Text2DPass(
                const core::vector<RenderPassBinding>& bindings,
                RenderTarget* output,
                GeometryStorage* geometryStorage
            );
            ~Text2DPass() override;

            virtual void Update(Scene *scene) override;
            virtual void Draw(Scene *scene) override;

        };

    }

}
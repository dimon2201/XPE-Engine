#pragma once

#include <rendering/draw/text_render_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API Text3DRenderPass : public TextRenderPass
        {

        public:
            Text3DRenderPass(
                const core::vector<RenderPassBinding>& bindings,
                RenderTarget* output,
                GeometryStorage* geometryStorage
            );
            ~Text3DRenderPass() override;

            virtual void Update(Scene* scene) override;
            virtual void Draw(Scene* scene) override;

        };

    }

}
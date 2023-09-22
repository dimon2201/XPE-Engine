#pragma once

#include <rendering/draw/text_render_pass.h>

namespace xpe {

    namespace render {

        class ViewportBuffer;

        class ENGINE_API Text2DRenderPass : public TextRenderPass
        {

        public:
            Text2DRenderPass(
                const core::vector<RenderPassBinding>& bindings,
                RenderTarget* output,
                GeometryStorage* geometryStorage
            );
            ~Text2DRenderPass() override;

            virtual void Update(Scene *scene) override;
            virtual void Draw(Scene *scene) override;

        };

    }

}
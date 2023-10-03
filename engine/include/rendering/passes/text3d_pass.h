#pragma once

#include <rendering/passes/text_pass.h>

namespace xpe {

    namespace render {

        class ENGINE_API Text3DPass : public TextPass
        {

        public:
            Text3DPass(
                const core::vector<RenderPassBinding>& bindings,
                GeometryStorage* geometryStorage,
                RenderTarget* output
            );
            ~Text3DPass() override;

            virtual void Update(Scene* scene) override;
            virtual void Draw(Scene* scene) override;

        };

    }

}
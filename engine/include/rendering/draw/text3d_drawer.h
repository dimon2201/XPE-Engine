#pragma once

#include <rendering/draw/text_drawer.h>

namespace xpe {

    namespace render {

        class ENGINE_API Text3DDrawer : public TextDrawer
        {

        public:

            Text3DDrawer(
                Shader* shader,
                RenderTarget* renderTarget,
                GeometryStorage* geometryStorage,
                const vector<Buffer*>& VSBuffers = {},
                const vector<Buffer*>& PSBuffers = {}
            );

            ~Text3DDrawer() override;

            void Draw(Scene *scene) override;

        };

    }

}
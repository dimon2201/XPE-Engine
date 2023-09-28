#pragma once

#include <rendering/draw/text_drawer.h>

namespace xpe {

    namespace render {

        class ViewportBuffer;

        class ENGINE_API Text2DDrawer : public TextDrawer
        {

        public:

            Text2DDrawer(
                Shader* shader,
                RenderTarget* renderTarget,
                GeometryStorage* geometryStorage,
                const vector<Buffer*>& VSBuffers = {},
                const vector<Buffer*>& PSBuffers = {}
            );

            ~Text2DDrawer() override;

            void Draw(Scene *scene) override;

        };

    }

}
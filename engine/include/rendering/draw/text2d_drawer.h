#pragma once

#include <rendering/draw/text_drawer.h>

namespace xpe {

    namespace render {

        class ViewportBuffer;

        class ENGINE_API Text2DDrawer : public TextDrawer
        {

        public:

            Text2DDrawer(
                CameraBuffer* cameraBuffer,
                Shader* shader,
                GeometryStorage* geometryStorage,
                ViewportBuffer* viewportBuffer,
                RenderTarget* renderTarget,
                Viewport* viewport
            );

            ~Text2DDrawer() override;

            void Draw(Scene *scene) override;

        };

    }

}
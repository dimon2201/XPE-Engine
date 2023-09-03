#pragma once

#include <rendering/draw/text_drawer.h>

namespace xpe {

    namespace render {

        class ENGINE_API Text3DDrawer : public TextDrawer
        {

        public:

            Text3DDrawer(
                CameraBuffer* cameraBuffer,
                Shader* shader,
                GeometryStorage* geometryStorage,
                RenderTarget* renderTarget
            );

            ~Text3DDrawer() override;

            void Draw(Scene *scene) override;

        };

    }

}
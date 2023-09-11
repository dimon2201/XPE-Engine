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
                RenderTarget* renderTarget,
                GeometryStorage* geometryStorage
            );

            ~Text3DDrawer() override;

            void Draw(Scene *scene) override;

        };

    }

}
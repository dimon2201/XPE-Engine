#pragma once

#include <rendering/texture.h>

namespace xpe {

    namespace render {

        struct ENGINE_API DepthStencilState final {
            GPUResource Instance;
            Boolean UseDepthTest;
        };

        struct ENGINE_API RenderTarget final {
            int Width = 0;
            int Height = 0;
            Texture* ColorTexture = nullptr;
            Texture* DepthTexture = nullptr;
            void* ColorTargetView = nullptr;
            void* DepthTargetView = nullptr;
        };

    }

}
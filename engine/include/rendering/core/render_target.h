#pragma once

#include <rendering/core/texture.h>

namespace xpe {

    namespace render {

        struct ENGINE_API DepthStencilState final {
            void* State = nullptr;
            Boolean UseDepthTest;
        };

        struct ENGINE_API BlendState final {
            void* State = nullptr;
            Boolean UseBlending;
        };

        struct ENGINE_API RenderTarget final {
            Texture* ColorTexture = nullptr;
            Texture* DepthTexture = nullptr;
            void* ColorTargetView = nullptr;
            void* DepthTargetView = nullptr;
        };

    }

}
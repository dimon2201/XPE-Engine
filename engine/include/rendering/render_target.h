#pragma once

#include <rendering/texture.h>

namespace xpe {

    namespace render {

        struct ENGINE_API DepthStencilState final {
            GPUResource Instance;
            Boolean UseDepthTest;
        };

        struct ENGINE_API RenderTarget final {
            Texture* ColorTexture;
            Texture* DepthTexture;
            void* ColorTargetInstance;
            void* DepthTargetInstance;
            glm::ivec2 Dimensions;
        };

    }

}
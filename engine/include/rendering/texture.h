#pragma once

#include <rendering/core.h>

namespace xpe {

    namespace render {

        enum class eTextureType {
            TEXTURE_1D = 1,
            TEXTURE_2D = 2,
            TEXTURE_3D = 3,
            TEXTURE_CUBE = 4,
            TEXTURE_ARRAY = 5,

            TEXTURE_DEFAULT = TEXTURE_2D
        };

        struct ENGINE_API Texture : public GPUResource {
            eTextureType Type = eTextureType::TEXTURE_DEFAULT;
        };

        struct ENGINE_API TextureSampler : public GPUResource {
            glm::vec4 BorderColor = { 0, 0, 0, 0 };
        };

    }

}
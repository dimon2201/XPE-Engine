#include <rendering/materials/material.h>

namespace xpe {

    namespace render {

        const MaterialFormat Material::K_ALBEDO_FORMAT = { eTextureFormat::RGBA8, 2048, 2048, K_SLOT_ALBEDO };
        const MaterialFormat Material::K_BUMP_FORMAT   = { eTextureFormat::RGB8, 2048, 2048, K_SLOT_BUMPING };
        const MaterialFormat Material::K_PARALLAX_FORMAT = { eTextureFormat::R32, 2048, 2048, K_SLOT_PARALLAX };
        const MaterialFormat Material::K_METALLIC_FORMAT = { eTextureFormat::R32, 2048, 2048, K_SLOT_METALLIC };
        const MaterialFormat Material::K_ROUGHNESS_FORMAT = { eTextureFormat::R32, 2048, 2048, K_SLOT_ROUGHNESS };
        const MaterialFormat Material::K_AO_FORMAT = { eTextureFormat::R32, 2048, 2048, K_SLOT_AO };
        const MaterialFormat Material::K_EMISSION_FORMAT = { eTextureFormat::RGB8, 2048, 2048, K_SLOT_EMISSION };

    }

}
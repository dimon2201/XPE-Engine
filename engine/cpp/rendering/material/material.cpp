#include <rendering/material/material.h>

namespace xpe {

    namespace render {

        const sMaterialFormat sMaterial::k_AlbedoFormat = {eTextureFormat::SRGBA8, 2048, 2048, K_SLOT_ALBEDO };
        const sMaterialFormat sMaterial::k_NormalFormat   = {eTextureFormat::RGBA8, 2048, 2048, K_SLOT_BUMPING };
        const sMaterialFormat sMaterial::k_ParallaxFormat = {eTextureFormat::R8, 2048, 2048, K_SLOT_PARALLAX };
        const sMaterialFormat sMaterial::k_MetalFormat = {eTextureFormat::R8, 2048, 2048, K_SLOT_METALLIC };
        const sMaterialFormat sMaterial::k_RoughnessFormat = {eTextureFormat::R8, 2048, 2048, K_SLOT_ROUGHNESS };
        const sMaterialFormat sMaterial::k_AoFormat = {eTextureFormat::R8, 2048, 2048, K_SLOT_AO };
        const sMaterialFormat sMaterial::k_EmissionFormat = {eTextureFormat::SRGBA8, 2048, 2048, K_SLOT_EMISSION };

    }

}
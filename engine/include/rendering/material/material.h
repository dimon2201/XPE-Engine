#pragma once

#include <rendering/buffers/material_data_buffer.h>

namespace xpe
{
    namespace render
    {
        using namespace core;

        struct ENGINE_API MaterialFormat final
        {
            eTextureFormat Format;
            usize Width;
            usize Height;
            u32 Slot;
        };

        struct ENGINE_API Material : public Object, MaterialData
        {
            static const MaterialFormat K_ALBEDO_FORMAT;
            static const MaterialFormat K_BUMP_FORMAT;
            static const MaterialFormat K_PARALLAX_FORMAT;
            static const MaterialFormat K_METALLIC_FORMAT;
            static const MaterialFormat K_ROUGHNESS_FORMAT;
            static const MaterialFormat K_AO_FORMAT;
            static const MaterialFormat K_EMISSION_FORMAT;

            u32 Index = 0;

            TextureLayer AlbedoMap;
            TextureLayer NormalMap;
            TextureLayer ParallaxMap;
            TextureLayer MetalMap;
            TextureLayer RoughnessMap;
            TextureLayer AOMap;
            TextureLayer EmissionMap;
        };

        Json(
            Material,
            Index,
            Albedo,
            EnableAlbedoMap,
            EnableNormalMap,
            EnableParallaxMap,
            ParallaxHeightScale,
            ParallaxMinLayers,
            ParallaxMaxLayers,
            Metallness,
            EnableMetalMap,
            Roughness,
            EnableRoughnessMap,
            AO,
            EnableAOMap,
            Emission,
            EnableEmissionMap
        )

    }

}
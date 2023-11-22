#pragma once

#include <rendering/buffers/material_data_buffer.h>

namespace xpe
{
    namespace render
    {
        using namespace core;

        struct ENGINE_API sMaterialFormat final
        {
            eTextureFormat Format;
            usize Width;
            usize Height;
            u32 Slot;
        };

        struct ENGINE_API sMaterial : public cObject, sMaterialData
        {
            static const sMaterialFormat k_AlbedoFormat;
            static const sMaterialFormat k_NormalFormat;
            static const sMaterialFormat k_ParallaxFormat;
            static const sMaterialFormat k_MetalFormat;
            static const sMaterialFormat k_RoughnessFormat;
            static const sMaterialFormat k_AoFormat;
            static const sMaterialFormat k_EmissionFormat;

            u32 Index = 0;

            sTextureLayer* AlbedoMap;
            sTextureLayer* NormalMap;
            sTextureLayer* ParallaxMap;
            sTextureLayer* MetalMap;
            sTextureLayer* RoughnessMap;
            sTextureLayer* AOMap;
            sTextureLayer* EmissionMap;
        };

        Json(
            sMaterial,
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
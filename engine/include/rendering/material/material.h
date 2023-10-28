#pragma once

#include <rendering/buffers/material_data_buffer.h>

namespace xpe
{
    namespace physics
    {
        struct sMaterial;
    }

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

            Ref<MaterialDataBuffer> Buffer;

            Ref<Texture> AlbedoAtlas;
            Ref<Texture> NormalAtlas;
            Ref<Texture> ParallaxAtlas;
            Ref<Texture> MetalAtlas;
            Ref<Texture> RoughnessAtlas;
            Ref<Texture> AOAtlas;
            Ref<Texture> EmissionAtlas;

            TextureLayer AlbedoMap;
            TextureLayer NormalMap;
            TextureLayer ParallaxMap;
            TextureLayer MetalMap;
            TextureLayer RoughnessMap;
            TextureLayer AOMap;
            TextureLayer EmissionMap;

            hstring AlbedoPath;
            hstring BumpingPath;
            hstring ParallaxPath;
            hstring MetallicPath;
            hstring RoughnessPath;
            hstring AOPath;
            hstring EmissionPath;

            core::f32 PhysicsStaticFriction;
            core::f32 PhysicsDynamicFriction;
            core::f32 PhysicsRestitution;
            physics::sMaterial* PhysicsMaterial;

            Material() = default;
            Material(const Material& material) = default;

            void Flush();
            void AddLayer(Texture& texture, TextureLayer& layer);
            void SetLayer(Texture& texture, TextureLayer& layer, u32 layerIndex);

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
            EnableEmissionMap,
            AlbedoPath,
            BumpingPath,
            ParallaxPath,
            MetallicPath,
            RoughnessPath,
            AOPath,
            EmissionPath
        )

    }

}
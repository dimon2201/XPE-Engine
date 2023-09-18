#pragma once

#include <rendering/buffers/material_data_buffer.h>

namespace xpe {

    namespace render {

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

            Ref<Texture> AlbedoArray;
            Ref<Texture> BumpArray;
            Ref<Texture> ParallaxArray;
            Ref<Texture> MetallicArray;
            Ref<Texture> RoughnessArray;
            Ref<Texture> AOArray;
            Ref<Texture> EmissionArray;

            TextureLayer Albedo;
            TextureLayer Bumping;
            TextureLayer Parallax;
            TextureLayer Metallic;
            TextureLayer Roughness;
            TextureLayer AO;
            TextureLayer Emission;

            hstring AlbedoPath;
            hstring BumpingPath;
            hstring ParallaxPath;
            hstring MetallicPath;
            hstring RoughnessPath;
            hstring AOPath;
            hstring EmissionPath;

            Material() = default;
            Material(const Material& material) = default;

            void Flush();
            void AddLayer(Texture& texture, TextureLayer& layer);
            void SetLayer(Texture& texture, TextureLayer& layer, u32 layerIndex);

        };

        Json(
            Material,
            Index,
            BaseColor,
            EnableAlbedo,
            EnableBumping,
            EnableParallax,
            HeightScale,
            ParallaxMinLayers,
            ParallaxMaxLayers,
            MetallicFactor,
            EnableMetallic,
            RoughnessFactor,
            EnableRoughness,
            AOFactor,
            EnableAO,
            EmissionColor,
            EnableEmission,
            AlbedoPath,
            BumpingPath,
            ParallaxPath,
            MetallicPath,
            RoughnessPath,
            AOPath,
            EmissionPath
        )

        struct ENGINE_API MaterialInstance : public Object
        {
            math::Transform Transform;
            Ref<Material> Material;

            MaterialInstance() = default;

            MaterialInstance(const math::Transform& transform, const Ref<render::Material>& material)
            : Transform(transform), Material(material) {}
        };

        Json(MaterialInstance, Transform)

    }

}
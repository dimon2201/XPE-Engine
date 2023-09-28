#include <rendering/materials/material.h>

namespace xpe {

    namespace render {

        const MaterialFormat Material::K_ALBEDO_FORMAT = { eTextureFormat::SRGBA8, 2048, 2048, K_SLOT_ALBEDO };
        const MaterialFormat Material::K_BUMP_FORMAT   = { eTextureFormat::RGBA8, 2048, 2048, K_SLOT_BUMPING };
        const MaterialFormat Material::K_PARALLAX_FORMAT = { eTextureFormat::R8, 2048, 2048, K_SLOT_PARALLAX };
        const MaterialFormat Material::K_METALLIC_FORMAT = { eTextureFormat::R8, 2048, 2048, K_SLOT_METALLIC };
        const MaterialFormat Material::K_ROUGHNESS_FORMAT = { eTextureFormat::R8, 2048, 2048, K_SLOT_ROUGHNESS };
        const MaterialFormat Material::K_AO_FORMAT = { eTextureFormat::R8, 2048, 2048, K_SLOT_AO };
        const MaterialFormat Material::K_EMISSION_FORMAT = { eTextureFormat::SRGBA8, 2048, 2048, K_SLOT_EMISSION };

        void Material::AddLayer(Texture &texture, TextureLayer &layer)
        {
            if (layer.Pixels == nullptr) {
                layer = texture.CreateLayer();
            }

//            if (layer.Mips.empty()) {
//                layer.GenerateMips(texture.Width, texture.Height, texture.Format);
//            }

            texture.Layers.emplace_back(layer);
            texture.Flush();
        }

        void Material::SetLayer(Texture &texture, TextureLayer &layer, u32 layerIndex)
        {
            if (layer.Pixels == nullptr) {
                layer = texture.CreateLayer();
            }

//            if (layer.Mips.empty()) {
//                layer.GenerateMips(texture.Width, texture.Height, texture.Format);
//            }

            texture.Layers[layerIndex] = layer;
            texture.Flush();
        }

        void Material::Flush()
        {
            Buffer->FlushItem(Index, *this);
        }

    }

}
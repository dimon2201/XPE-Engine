#include <rendering/material/material_manager.hpp>

namespace xpe {

    namespace render {

        void cMaterialManager::Init()
        {
            Buffers::Material = new sMaterialDataBuffer();
            InitSampler();
            Textures::AlbedoAtlas = InitTextureArray(sMaterial::k_AlbedoFormat);
            Textures::NormalAtlas = InitTextureArray(sMaterial::k_NormalFormat);
            Textures::ParallaxAtlas = InitTextureArray(sMaterial::k_ParallaxFormat);
            Textures::MetalAtlas = InitTextureArray(sMaterial::k_MetalFormat);
            Textures::RoughnessAtlas = InitTextureArray(sMaterial::k_RoughnessFormat);
            Textures::AOAtlas = InitTextureArray(sMaterial::k_AoFormat);
            Textures::EmissionAtlas = InitTextureArray(sMaterial::k_EmissionFormat);
        }

        void cMaterialManager::Free()
        {
            delete Buffers::Material;
            delete Textures::AlbedoAtlas;
            delete Textures::NormalAtlas;
            delete Textures::ParallaxAtlas;
            delete Textures::MetalAtlas;
            delete Textures::RoughnessAtlas;
            delete Textures::AOAtlas;
            delete Textures::EmissionAtlas;
            delete Samplers::Material;
        }

        void cMaterialManager::InitSampler()
        {
            Samplers::Material                  = new sSampler();
            Samplers::Material->Slot            = K_SLOT_MATERIAL_SAMPLER;
            Samplers::Material->Filter          = sSampler::eFilter::ANISOTROPIC;
            Samplers::Material->AnisotropyLevel = cHardwareManager::GPU.MaxAnisotropyLevel;
            Samplers::Material->AddressU        = sSampler::eAddress::WRAP;
            Samplers::Material->AddressV        = sSampler::eAddress::WRAP;
            Samplers::Material->AddressW        = sSampler::eAddress::WRAP;
            Samplers::Material->Init();
        }

        sTexture* cMaterialManager::InitTextureArray(const sMaterialFormat& materialFormat)
        {
            sTexture* texture = new sTexture();
            texture->InitializeData = true;
            texture->Type = sTexture::eType::TEXTURE_2D_ARRAY;
            texture->Usage = sTexture::eUsage::DEFAULT;
            texture->Format = materialFormat.Format;
            texture->Width = materialFormat.Width;
            texture->Height = materialFormat.Height;
            texture->Slot = materialFormat.Slot;
            texture->Channels = sTexture::k_ChannelTable.at(materialFormat.Format);
            texture->Layers.reserve(cHardwareManager::GPU.MaxTexture2dArray);
            return texture;
        }

        void cMaterialManager::Clear()
        {
            Buffers::Material->Clear();
            Buffers::Material->Flush();

            Textures::AlbedoAtlas->Layers.clear();
            Textures::AlbedoAtlas->Flush();

            Textures::NormalAtlas->Layers.clear();
            Textures::NormalAtlas->Flush();

            Textures::ParallaxAtlas->Layers.clear();
            Textures::ParallaxAtlas->Flush();

            Textures::MetalAtlas->Layers.clear();
            Textures::MetalAtlas->Flush();

            Textures::RoughnessAtlas->Layers.clear();
            Textures::RoughnessAtlas->Flush();

            Textures::AOAtlas->Layers.clear();
            Textures::AOAtlas->Flush();

            Textures::EmissionAtlas->Layers.clear();
            Textures::EmissionAtlas->Flush();
        }

        void cMaterialManager::Flush(const sMaterial& material)
        {
            Buffers::Material->FlushItem(material.Index, material);
        }

        void cMaterialManager::AddLayer(sTexture &texture, sTextureLayer* layer)
        {
            if (layer == nullptr || layer->Pixels == nullptr) {
                texture.Layers.emplace_back(texture.CreateLayer());
            } else {
                texture.Layers.emplace_back(*layer);
//                layer.ResizePixels(texture.Format, texture.Width, texture.Height);
            }

//            if (layer.Mips.empty()) {
//                layer.GenerateMips(texture.Format, texture.Width, texture.Height);
//            }

            texture.Flush();
        }

        void cMaterialManager::SetLayer(sTexture &texture, sTextureLayer* layer, u32 layerIndex)
        {
            if (layer == nullptr || layer->Pixels == nullptr) {
                texture.Layers[layerIndex] = texture.CreateLayer();
            } else {
                texture.Layers[layerIndex] = *layer;
            //                layer.ResizePixels(texture.Format, texture.Width, texture.Height);
            }

            //            if (layer.Mips.empty()) {
            //                layer.GenerateMips(texture.Format, texture.Width, texture.Height);
            //            }

            texture.Flush();
        }

    }

}
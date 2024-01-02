#include <core/types.hpp>
#include <rendering/material/material_manager.hpp>

namespace xpe {

    namespace render {

        void cMaterialManager::Init()
        {
            Buffers::Material = new cMaterialDataBuffer(1024);
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

        cTexture* cMaterialManager::InitTextureArray(const sMaterialFormat& materialFormat)
        {
            cTexture* texture = new cTexture();
            texture->SetInitializeData(true);
            texture->SetType(cTexture::eType::TEXTURE_2D_ARRAY);
            texture->SetUsage(cTexture::eUsage::DEFAULT);
            texture->SetFormat(materialFormat.Format);
            texture->SetWidth(materialFormat.Width);
            texture->SetHeight(materialFormat.Height);
            texture->SetSlot(materialFormat.Slot);
            texture->SetChannelCount(cTexture::k_ChannelTable.at(materialFormat.Format));
            texture->GetLayers().reserve(cHardwareManager::GPU.MaxTexture2dArray);
            return texture;
        }

        void cMaterialManager::Clear()
        {
            Buffers::Material->Clear();
            Buffers::Material->Flush();

            Textures::AlbedoAtlas->GetLayers().clear();
            Textures::AlbedoAtlas->Flush();

            Textures::NormalAtlas->GetLayers().clear();
            Textures::NormalAtlas->Flush();

            Textures::ParallaxAtlas->GetLayers().clear();
            Textures::ParallaxAtlas->Flush();

            Textures::MetalAtlas->GetLayers().clear();
            Textures::MetalAtlas->Flush();

            Textures::RoughnessAtlas->GetLayers().clear();
            Textures::RoughnessAtlas->Flush();

            Textures::AOAtlas->GetLayers().clear();
            Textures::AOAtlas->Flush();

            Textures::EmissionAtlas->GetLayers().clear();
            Textures::EmissionAtlas->Flush();
        }

        void cMaterialManager::Flush(const sMaterial& material)
        {
            Buffers::Material->FlushItem(material.Index, material);
        }

        void cMaterialManager::AddLayer(cTexture &texture, sTextureLayer* layer)
        {
            if (layer == nullptr || layer->Pixels == nullptr) {
                texture.GetLayers().emplace_back(texture.CreateLayer());
            } else {
                texture.GetLayers().emplace_back(*layer);
//                layer.ResizePixels(texture.Format, texture.Width, texture.Height);
            }

//            if (layer.Mips.empty()) {
//                layer.GenerateMips(texture.Format, texture.Width, texture.Height);
//            }

            texture.Flush();
        }

        void cMaterialManager::SetLayer(cTexture &texture, sTextureLayer* layer, u32 layerIndex)
        {
            if (layer == nullptr || layer->Pixels == nullptr) {
                texture.GetLayers()[layerIndex] = texture.CreateLayer();
            } else {
                texture.GetLayers()[layerIndex] = *layer;
            //                layer.ResizePixels(texture.Format, texture.Width, texture.Height);
            }

            //            if (layer.Mips.empty()) {
            //                layer.GenerateMips(texture.Format, texture.Width, texture.Height);
            //            }

            texture.Flush();
        }

    }

}
#include <rendering/material/material_manager.h>

namespace xpe {

    namespace render {

        unordered_map<string, Ref<Material>>* MaterialManager::s_Map = nullptr;
        Ref<MaterialDataBuffer> MaterialManager::s_DataBuffer;

        TextureSampler MaterialManager::Sampler;
        Ref<Texture> MaterialManager::AlbedoAtlas;
        Ref<Texture> MaterialManager::NormalAtlas;
        Ref<Texture> MaterialManager::ParallaxAtlas;
        Ref<Texture> MaterialManager::MetalAtlas;
        Ref<Texture> MaterialManager::RoughnessAtlas;
        Ref<Texture> MaterialManager::AOAtlas;
        Ref<Texture> MaterialManager::EmissionAtlas;

        void MaterialManager::Init()
        {
            s_Map = new unordered_map<string, Ref<Material>>();
            s_DataBuffer.Create();
            InitSampler();
            AlbedoAtlas = InitTextureArray(Material::K_ALBEDO_FORMAT);
            NormalAtlas = InitTextureArray(Material::K_BUMP_FORMAT);
            ParallaxAtlas = InitTextureArray(Material::K_PARALLAX_FORMAT);
            MetalAtlas = InitTextureArray(Material::K_METALLIC_FORMAT);
            RoughnessAtlas = InitTextureArray(Material::K_ROUGHNESS_FORMAT);
            AOAtlas = InitTextureArray(Material::K_AO_FORMAT);
            EmissionAtlas = InitTextureArray(Material::K_EMISSION_FORMAT);
        }

        void MaterialManager::Free()
        {
            s_DataBuffer.Destroy();
            AlbedoAtlas.Destroy();
            NormalAtlas.Destroy();
            ParallaxAtlas.Destroy();
            MetalAtlas.Destroy();
            RoughnessAtlas.Destroy();
            AOAtlas.Destroy();
            EmissionAtlas.Destroy();
            context::FreeSampler(Sampler);
        }

        void MaterialManager::InitSampler()
        {
            Sampler.Slot            = K_SLOT_MATERIAL_SAMPLER;
            Sampler.Filter          = TextureSampler::eFilter::ANISOTROPIC;
            Sampler.AnisotropyLevel = Hardware::GPU.MaxAnisotropyLevel;
            Sampler.AddressU        = TextureSampler::eAddress::WRAP;
            Sampler.AddressV        = TextureSampler::eAddress::WRAP;
            Sampler.AddressW        = TextureSampler::eAddress::WRAP;

            context::CreateSampler(Sampler);
        }

        Ref<Texture> MaterialManager::InitTextureArray(const MaterialFormat& materialFormat)
        {
            Ref<Texture> texture;
            texture.Create();

            texture->InitializeData = true;
            texture->Type = Texture::eType::TEXTURE_2D_ARRAY;
            texture->Usage = Texture::eUsage::DEFAULT;
            texture->Format = materialFormat.Format;
            texture->Width = materialFormat.Width;
            texture->Height = materialFormat.Height;
            texture->Slot = materialFormat.Slot;
            texture->Channels = Texture::ChannelTable.at(materialFormat.Format);
            texture->Layers.reserve(Hardware::GPU.MaxTexture2dArray);

            return texture;
        }

        void MaterialManager::Remove(const string &name)
        {
            auto it = s_Map->find(name);
            if (it != s_Map->end()) {
                u32 index = it->second->Index;
                s_Map->erase(it);

                s_DataBuffer->RemoveAt(index);
                s_DataBuffer->Flush();

                AlbedoAtlas->RemoveLayerAt(index);
                AlbedoAtlas->Flush();

                NormalAtlas->RemoveLayerAt(index);
                NormalAtlas->Flush();

                ParallaxAtlas->RemoveLayerAt(index);
                ParallaxAtlas->Flush();

                MetalAtlas->RemoveLayerAt(index);
                MetalAtlas->Flush();

                RoughnessAtlas->RemoveLayerAt(index);
                RoughnessAtlas->Flush();

                AOAtlas->RemoveLayerAt(index);
                AOAtlas->Flush();

                EmissionAtlas->RemoveLayerAt(index);
                EmissionAtlas->Flush();
            }
        }

        void MaterialManager::Clear()
        {
            s_Map->clear();

            s_DataBuffer->Clear();
            s_DataBuffer->Flush();

            AlbedoAtlas->Layers.clear();
            AlbedoAtlas->Flush();

            NormalAtlas->Layers.clear();
            NormalAtlas->Flush();

            ParallaxAtlas->Layers.clear();
            ParallaxAtlas->Flush();

            MetalAtlas->Layers.clear();
            MetalAtlas->Flush();

            RoughnessAtlas->Layers.clear();
            RoughnessAtlas->Flush();

            AOAtlas->Layers.clear();
            AOAtlas->Flush();

            EmissionAtlas->Layers.clear();
            EmissionAtlas->Flush();
        }

        void MaterialManager::Bind(Pipeline& pipeline)
        {
            pipeline.PSBuffers.emplace_back(s_DataBuffer.Get());

            pipeline.Samplers.emplace_back(&Sampler);

            pipeline.Textures.emplace_back(AlbedoAtlas.Get());
            pipeline.Textures.emplace_back(NormalAtlas.Get());
            pipeline.Textures.emplace_back(ParallaxAtlas.Get());
            pipeline.Textures.emplace_back(MetalAtlas.Get());
            pipeline.Textures.emplace_back(RoughnessAtlas.Get());
            pipeline.Textures.emplace_back(AOAtlas.Get());
            pipeline.Textures.emplace_back(EmissionAtlas.Get());
        }

    }

}
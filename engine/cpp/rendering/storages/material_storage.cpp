#include <rendering/storages/material_storage.h>

namespace xpe {

    namespace render {

        MaterialStorage::MaterialStorage()
        {
            m_DataBuffer.Create();
            InitSampler();
            AlbedoAtlas = InitTextureArray(Material::K_ALBEDO_FORMAT);
            NormalAtlas = InitTextureArray(Material::K_BUMP_FORMAT);
            ParallaxAtlas = InitTextureArray(Material::K_PARALLAX_FORMAT);
            MetalAtlas = InitTextureArray(Material::K_METALLIC_FORMAT);
            RoughnessAtlas = InitTextureArray(Material::K_ROUGHNESS_FORMAT);
            AOAtlas = InitTextureArray(Material::K_AO_FORMAT);
            EmissionAtlas = InitTextureArray(Material::K_EMISSION_FORMAT);
        }

        MaterialStorage::~MaterialStorage()
        {
            context::FreeSampler(Sampler);
        }

        void MaterialStorage::InitSampler()
        {
            Sampler.Slot            = K_SLOT_MATERIAL_SAMPLER;
            Sampler.Filter          = TextureSampler::eFilter::ANISOTROPIC;
            Sampler.AnisotropyLevel = Hardware::GPU.MaxAnisotropyLevel;
            Sampler.AddressU        = TextureSampler::eAddress::WRAP;
            Sampler.AddressV        = TextureSampler::eAddress::WRAP;
            Sampler.AddressW        = TextureSampler::eAddress::WRAP;

            context::CreateSampler(Sampler);
        }

        Ref<Texture> MaterialStorage::InitTextureArray(const MaterialFormat& materialFormat)
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

        void MaterialStorage::Remove(const string &name)
        {
            auto it = m_Map.find(name);
            if (it != m_Map.end()) {
                u32 index = it->second->Index;
                m_Map.erase(it);

                m_DataBuffer->RemoveAt(index);
                m_DataBuffer->Flush();

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

        void MaterialStorage::Clear()
        {
            m_Map.clear();

            m_DataBuffer->Clear();
            m_DataBuffer->Flush();

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

        void MaterialStorage::BindPipeline(Pipeline& pipeline)
        {
            pipeline.PSBuffers.emplace_back(m_DataBuffer.Get());

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
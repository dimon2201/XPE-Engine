#include <rendering/storages/material_storage.h>

namespace xpe {

    namespace render {

        MaterialStorage::MaterialStorage()
        {
            InitSampler();
            AlbedoArray = InitTextureArray(Material::K_ALBEDO_FORMAT);
            BumpArray = InitTextureArray(Material::K_BUMP_FORMAT);
            ParallaxArray = InitTextureArray(Material::K_PARALLAX_FORMAT);
            MetallicArray = InitTextureArray(Material::K_METALLIC_FORMAT);
            RoughnessArray = InitTextureArray(Material::K_ROUGHNESS_FORMAT);
            AOArray = InitTextureArray(Material::K_AO_FORMAT);
            EmissionArray = InitTextureArray(Material::K_EMISSION_FORMAT);
        }

        MaterialStorage::~MaterialStorage()
        {
            context::FreeSampler(Sampler);
        }

        void MaterialStorage::InitSampler()
        {
            TextureSampler& materialSampler = Sampler;
            materialSampler.Filter          = TextureSampler::eFilter::ANISOTROPIC;
            materialSampler.AnisotropyLevel = os::HardwareConfig::GetVideoStats().MaxAnisotropyLevel;
            materialSampler.AddressU        = TextureSampler::eAddress::WRAP;
            materialSampler.AddressV        = TextureSampler::eAddress::WRAP;
            materialSampler.AddressW        = TextureSampler::eAddress::WRAP;

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
            texture->Layers.reserve(os::HardwareConfig::GetVideoStats().MaxTexture2dArray);

            return texture;
        }

        u32 MaterialStorage::GetIndex(Material* material)
        {
            if (material == nullptr) return 0;

            auto it = m_MaterialIndices.find(material);
            if (it != m_MaterialIndices.end()) {
                return it->second;
            }

            return 0;
        }

        void MaterialStorage::Remove(const string &name)
        {
            auto it = m_Materials.find(name);
            if (it != m_Materials.end()) {
                u32 index = m_MaterialIndices[it->second.Get()];
                m_Materials.erase(it);

                m_DataBuffer.RemoveAt(index);
                m_DataBuffer.Flush();

                AlbedoArray->RemoveLayerAt(index);
                AlbedoArray->Flush();

                BumpArray->RemoveLayerAt(index);
                BumpArray->Flush();

                ParallaxArray->RemoveLayerAt(index);
                ParallaxArray->Flush();

                MetallicArray->RemoveLayerAt(index);
                MetallicArray->Flush();

                RoughnessArray->RemoveLayerAt(index);
                RoughnessArray->Flush();

                AOArray->RemoveLayerAt(index);
                AOArray->Flush();

                EmissionArray->RemoveLayerAt(index);
                EmissionArray->Flush();
            }
        }

        void MaterialStorage::Clear()
        {
            m_Materials.clear();
            m_MaterialIndices.clear();

            m_DataBuffer.Clear();
            m_DataBuffer.Flush();

            AlbedoArray->Layers.clear();
            AlbedoArray->Flush();

            BumpArray->Layers.clear();
            BumpArray->Flush();

            ParallaxArray->Layers.clear();
            ParallaxArray->Flush();

            MetallicArray->Layers.clear();
            MetallicArray->Flush();

            RoughnessArray->Layers.clear();
            RoughnessArray->Flush();

            AOArray->Layers.clear();
            AOArray->Flush();

            EmissionArray->Layers.clear();
            EmissionArray->Flush();
        }

        void MaterialStorage::BindPipeline(Pipeline& pipeline)
        {
            pipeline.PSBuffers.emplace_back(&m_DataBuffer);

            pipeline.Samplers.emplace_back(&Sampler);

            pipeline.Textures.emplace_back(AlbedoArray.Get());
            pipeline.Textures.emplace_back(BumpArray.Get());
            pipeline.Textures.emplace_back(ParallaxArray.Get());
            pipeline.Textures.emplace_back(MetallicArray.Get());
            pipeline.Textures.emplace_back(RoughnessArray.Get());
            pipeline.Textures.emplace_back(AOArray.Get());
            pipeline.Textures.emplace_back(EmissionArray.Get());
        }

        void MaterialStorage::AddLayer(Texture &texture, TextureLayer &layer)
        {
            if (layer.Pixels != nullptr) {
                if (layer.Mips.empty()) {
                    layer.GenerateMips(texture.Width, texture.Height, texture.Format);
                }
                texture.Layers.emplace_back(layer);
                texture.Flush();
            }
        }

        void MaterialStorage::SetLayer(Texture &texture, TextureLayer &layer, u32 layerIndex)
        {
            if (layer.Pixels != nullptr) {
                if (layer.Mips.empty()) {
                    layer.GenerateMips(texture.Width, texture.Height, texture.Format);
                }
                texture.Layers[layerIndex] = layer;
                texture.Flush();
            }
        }

    }

}
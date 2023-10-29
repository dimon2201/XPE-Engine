#include <rendering/material/material_manager.h>

namespace xpe {

    namespace render {

        MaterialDataBuffer* MaterialManager::s_DataBuffer = nullptr;

        TextureSampler MaterialManager::Sampler;
        Texture* MaterialManager::AlbedoAtlas;
        Texture* MaterialManager::NormalAtlas;
        Texture* MaterialManager::ParallaxAtlas;
        Texture* MaterialManager::MetalAtlas;
        Texture* MaterialManager::RoughnessAtlas;
        Texture* MaterialManager::AOAtlas;
        Texture* MaterialManager::EmissionAtlas;

        void MaterialManager::Init()
        {
            s_DataBuffer = new MaterialDataBuffer();
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
            delete s_DataBuffer;
            delete AlbedoAtlas;
            delete NormalAtlas;
            delete ParallaxAtlas;
            delete MetalAtlas;
            delete RoughnessAtlas;
            delete AOAtlas;
            delete EmissionAtlas;
            context::FreeSampler(Sampler);
        }

        void MaterialManager::InitSampler()
        {
            Sampler.Slot            = K_SLOT_MATERIAL_SAMPLER;
            Sampler.Filter          = TextureSampler::eFilter::ANISOTROPIC;
            Sampler.AnisotropyLevel = HardwareManager::GPU.MaxAnisotropyLevel;
            Sampler.AddressU        = TextureSampler::eAddress::WRAP;
            Sampler.AddressV        = TextureSampler::eAddress::WRAP;
            Sampler.AddressW        = TextureSampler::eAddress::WRAP;

            context::CreateSampler(Sampler);
        }

        Texture* MaterialManager::InitTextureArray(const MaterialFormat& materialFormat)
        {
            Texture* texture = new Texture();
            texture->InitializeData = true;
            texture->Type = Texture::eType::TEXTURE_2D_ARRAY;
            texture->Usage = Texture::eUsage::DEFAULT;
            texture->Format = materialFormat.Format;
            texture->Width = materialFormat.Width;
            texture->Height = materialFormat.Height;
            texture->Slot = materialFormat.Slot;
            texture->Channels = Texture::ChannelTable.at(materialFormat.Format);
            texture->Layers.reserve(HardwareManager::GPU.MaxTexture2dArray);
            return texture;
        }

        void MaterialManager::Clear()
        {
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
            pipeline.PSBuffers.emplace_back(s_DataBuffer);

            pipeline.Samplers.emplace_back(&Sampler);

            pipeline.Textures.emplace_back(AlbedoAtlas);
            pipeline.Textures.emplace_back(NormalAtlas);
            pipeline.Textures.emplace_back(ParallaxAtlas);
            pipeline.Textures.emplace_back(MetalAtlas);
            pipeline.Textures.emplace_back(RoughnessAtlas);
            pipeline.Textures.emplace_back(AOAtlas);
            pipeline.Textures.emplace_back(EmissionAtlas);
        }

        void MaterialManager::Flush(const Material& material)
        {
            s_DataBuffer->FlushItem(material.Index, material);
        }

        void MaterialManager::AddLayer(Texture &texture, TextureLayer &layer)
        {
            if (layer.Pixels == nullptr) {
                layer = texture.CreateLayer();
            } else {
//                layer.Resize(texture.Format, texture.Width, texture.Height);
            }
            texture.Layers.emplace_back(layer);

//            if (layer.Mips.empty()) {
//                layer.GenerateMips(texture.Format, texture.Width, texture.Height);
//            }

            texture.Flush();
        }

        void MaterialManager::SetLayer(Texture &texture, TextureLayer &layer, u32 layerIndex)
        {
            if (layer.Pixels == nullptr) {
                layer = texture.CreateLayer();
            } else {
            //                layer.Resize(texture.Format, texture.Width, texture.Height);
            }
            texture.Layers[layerIndex] = layer;

            //            if (layer.Mips.empty()) {
            //                layer.GenerateMips(texture.Format, texture.Width, texture.Height);
            //            }

            texture.Flush();
        }

    }

}
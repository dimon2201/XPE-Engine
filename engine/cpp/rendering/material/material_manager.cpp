#include <rendering/material/material_manager.h>

namespace xpe {

    namespace render {

        sMaterialDataBuffer* cMaterialManager::s_DataBuffer = nullptr;

        sSampler cMaterialManager::Sampler;
        sTexture* cMaterialManager::AlbedoAtlas;
        sTexture* cMaterialManager::NormalAtlas;
        sTexture* cMaterialManager::ParallaxAtlas;
        sTexture* cMaterialManager::MetalAtlas;
        sTexture* cMaterialManager::RoughnessAtlas;
        sTexture* cMaterialManager::AOAtlas;
        sTexture* cMaterialManager::EmissionAtlas;

        void cMaterialManager::Init()
        {
            s_DataBuffer = new sMaterialDataBuffer();
            InitSampler();
            AlbedoAtlas = InitTextureArray(sMaterial::k_AlbedoFormat);
            NormalAtlas = InitTextureArray(sMaterial::k_NormalFormat);
            ParallaxAtlas = InitTextureArray(sMaterial::k_ParallaxFormat);
            MetalAtlas = InitTextureArray(sMaterial::k_MetalFormat);
            RoughnessAtlas = InitTextureArray(sMaterial::k_RoughnessFormat);
            AOAtlas = InitTextureArray(sMaterial::k_AoFormat);
            EmissionAtlas = InitTextureArray(sMaterial::k_EmissionFormat);
        }

        void cMaterialManager::Free()
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

        void cMaterialManager::InitSampler()
        {
            Sampler.Slot            = K_SLOT_MATERIAL_SAMPLER;
            Sampler.Filter          = sSampler::eFilter::ANISOTROPIC;
            Sampler.AnisotropyLevel = cHardwareManager::GPU.MaxAnisotropyLevel;
            Sampler.AddressU        = sSampler::eAddress::WRAP;
            Sampler.AddressV        = sSampler::eAddress::WRAP;
            Sampler.AddressW        = sSampler::eAddress::WRAP;

            context::CreateSampler(Sampler);
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

        void cMaterialManager::Bind(sVertexPipeline& pipeline)
        {
            for (auto* stage : pipeline.Shader->Stages)
            {
                if (stage->Type == sShaderStage::eType::PIXEL) {
                    stage->Buffers.emplace_back(s_DataBuffer);

                    stage->Samplers.emplace_back(&Sampler);

                    stage->Textures.emplace_back(AlbedoAtlas);
                    stage->Textures.emplace_back(NormalAtlas);
                    stage->Textures.emplace_back(ParallaxAtlas);
                    stage->Textures.emplace_back(MetalAtlas);
                    stage->Textures.emplace_back(RoughnessAtlas);
                    stage->Textures.emplace_back(AOAtlas);
                    stage->Textures.emplace_back(EmissionAtlas);
                }
            }
        }

        void cMaterialManager::Flush(const sMaterial& material)
        {
            s_DataBuffer->FlushItem(material.Index, material);
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
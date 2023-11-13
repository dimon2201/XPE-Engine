#include <rendering/widget_manager.h>

namespace xpe {

    namespace render {

        sSampler cWidgetManager::s_TextureSampler;
        sTexture* cWidgetManager::s_TextureAtlas;

        void cWidgetManager::Init()
        {
            s_TextureSampler.Slot            = K_SLOT_WIDGET_SAMPLER;
            s_TextureSampler.Filter          = sSampler::eFilter::MIN_MAG_MIP_LINEAR;
            s_TextureSampler.AddressU        = sSampler::eAddress::WRAP;
            s_TextureSampler.AddressV        = sSampler::eAddress::WRAP;
            s_TextureSampler.AddressW        = sSampler::eAddress::WRAP;
            context::CreateSampler(s_TextureSampler);

            s_TextureAtlas = new sTexture();
            s_TextureAtlas->InitializeData = true;
            s_TextureAtlas->Type = sTexture::eType::TEXTURE_2D_ARRAY;
            s_TextureAtlas->Usage = sTexture::eUsage::DEFAULT;
            s_TextureAtlas->Format = eTextureFormat::RGBA8;
            s_TextureAtlas->Width = 512;
            s_TextureAtlas->Height = 512;
            s_TextureAtlas->Slot = K_SLOT_WIDGET_ATLAS;
            s_TextureAtlas->Channels = sTexture::k_ChannelTable.at(s_TextureAtlas->Format);
            s_TextureAtlas->Layers.reserve(cHardwareManager::GPU.MaxTexture2dArray);
        }

        void cWidgetManager::Free()
        {
            context::FreeSampler(s_TextureSampler);
            delete s_TextureAtlas;
        }

        void cWidgetManager::Clear()
        {
            s_TextureAtlas->Layers.clear();
            s_TextureAtlas->Flush();
        }

        void cWidgetManager::Bind(sPipeline& pipeline)
        {
            pipeline.Textures.emplace_back(s_TextureAtlas);
            pipeline.Samplers.emplace_back(&s_TextureSampler);
        }

    }

}
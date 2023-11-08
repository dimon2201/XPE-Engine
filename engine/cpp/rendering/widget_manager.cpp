#include <rendering/widget_manager.h>

namespace xpe {

    namespace render {

        sSampler cWidgetManager::s_Sampler;
        sTexture* cWidgetManager::s_TextureAtlas;

        void sWidget::UpdateXmlChildren()
        {
            XmlChildren.clear();
            for (auto* child : Children) {
                XmlChildren.emplace_back(child);
            }
        }

        void cWidgetManager::Init()
        {
            s_Sampler.Slot            = K_SLOT_WIDGET_SAMPLER;
            s_Sampler.Filter          = sSampler::eFilter::MIN_MAG_MIP_LINEAR;
            s_Sampler.AddressU        = sSampler::eAddress::WRAP;
            s_Sampler.AddressV        = sSampler::eAddress::WRAP;
            s_Sampler.AddressW        = sSampler::eAddress::WRAP;
            context::CreateSampler(s_Sampler);

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
            context::FreeSampler(s_Sampler);
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
            pipeline.Samplers.emplace_back(&s_Sampler);
        }

    }

}
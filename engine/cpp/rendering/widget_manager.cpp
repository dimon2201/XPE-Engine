#include <rendering/widget_manager.h>
#include <rendering/bindings.h>

namespace xpe {

    namespace render {

        void cWidgetManager::Init()
        {
            Samplers::Widget               = new sSampler();
            Samplers::Widget->Slot         = K_SLOT_WIDGET_SAMPLER;
            Samplers::Widget->Filter       = sSampler::eFilter::MIN_MAG_MIP_LINEAR;
            Samplers::Widget->AddressU     = sSampler::eAddress::WRAP;
            Samplers::Widget->AddressV     = sSampler::eAddress::WRAP;
            Samplers::Widget->AddressW     = sSampler::eAddress::WRAP;
            Samplers::Widget->Init();

            Textures::WidgetAtlas = new sAtlas();
            Textures::WidgetAtlas->InitializeData = true;
            Textures::WidgetAtlas->Type = sTexture::eType::TEXTURE_2D;
            Textures::WidgetAtlas->Usage = sTexture::eUsage::DEFAULT;
            Textures::WidgetAtlas->Format = eTextureFormat::RGBA8;
            Textures::WidgetAtlas->Width = 1024;
            Textures::WidgetAtlas->Height = 1024;
            Textures::WidgetAtlas->Channels = 4;
            Textures::WidgetAtlas->Slot = K_SLOT_WIDGET_ATLAS;
            Textures::WidgetAtlas->AddLayer();
            Textures::WidgetAtlas->Init();
        }

        void cWidgetManager::Free()
        {
            delete Samplers::Widget;
            delete Textures::WidgetAtlas;
        }

    }

}
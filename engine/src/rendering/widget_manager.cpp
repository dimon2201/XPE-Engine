#include <rendering/widget_manager.hpp>
#include <rendering/bindings.hpp>

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

            //Textures::WidgetAtlas = new sAtlas();
            //Textures::WidgetAtlas->SetInitializeData(true);
            //Textures::WidgetAtlas->SetType(cTexture::eType::TEXTURE_2D);
            //Textures::WidgetAtlas->SetUsage(cTexture::eUsage::DEFAULT);
            //Textures::WidgetAtlas->SetFormat(eTextureFormat::RGBA8);
            //Textures::WidgetAtlas->SetWidth(1024);
            //Textures::WidgetAtlas->SetHeight(1024);
            //Textures::WidgetAtlas->SetChannelCount(4);
            //Textures::WidgetAtlas->SetSlot(K_SLOT_WIDGET_ATLAS);
            //Textures::WidgetAtlas->AddLayer();
            //Textures::WidgetAtlas->Init();
        }

        void cWidgetManager::Free()
        {
            delete Samplers::Widget;
            delete Textures::WidgetAtlas;
        }

    }

}
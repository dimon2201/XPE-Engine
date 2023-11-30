#include <rendering/widget_manager.h>

namespace xpe {

    namespace render {

        sSampler cWidgetManager::s_WidgetSampler;
        sAtlas* cWidgetManager::s_WidgetAtlas;

        void cWidgetManager::Init()
        {
            s_WidgetSampler.Slot            = K_SLOT_WIDGET_SAMPLER;
            s_WidgetSampler.Filter          = sSampler::eFilter::MIN_MAG_MIP_LINEAR;
            s_WidgetSampler.AddressU        = sSampler::eAddress::WRAP;
            s_WidgetSampler.AddressV        = sSampler::eAddress::WRAP;
            s_WidgetSampler.AddressW        = sSampler::eAddress::WRAP;
            context::CreateSampler(s_WidgetSampler);

            s_WidgetAtlas = new sAtlas();
            s_WidgetAtlas->InitializeData = true;
            s_WidgetAtlas->Type = sTexture::eType::TEXTURE_2D;
            s_WidgetAtlas->Usage = sTexture::eUsage::DEFAULT;
            s_WidgetAtlas->Format = eTextureFormat::RGBA8;
            s_WidgetAtlas->Width = 1024;
            s_WidgetAtlas->Height = 1024;
            s_WidgetAtlas->Channels = 4;
            s_WidgetAtlas->Slot = K_SLOT_WIDGET_ATLAS;
            s_WidgetAtlas->AddLayer();
            s_WidgetAtlas->Init();
        }

        void cWidgetManager::Free()
        {
            context::FreeSampler(s_WidgetSampler);
            delete s_WidgetAtlas;
        }

        void cWidgetManager::Bind(sVertexPipeline& pipeline)
        {
            for (auto* stage : pipeline.Shader->Stages)
            {
                if (stage->Type == sShaderStage::eType::PIXEL) {
                    stage->Textures.emplace_back(s_WidgetAtlas);
                    stage->Samplers.emplace_back(&s_WidgetSampler);
                }
            }
        }

        sAtlas* cWidgetManager::GetAtlas()
        {
            return s_WidgetAtlas;
        }

    }

}
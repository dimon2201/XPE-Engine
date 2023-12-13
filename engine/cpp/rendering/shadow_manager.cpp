#include <rendering/shadow_manager.h>
#include <rendering/bindings.h>

namespace xpe {

    namespace render {

        void cShadowManager::Init()
        {
            Buffers::ShadowPCF = new sShadowPcfBuffer();

            Samplers::Shadow              = new sSampler();
            Samplers::Shadow->Slot        = K_SLOT_SHADOW_SAMPLER;
            Samplers::Shadow->Filter      = sSampler::eFilter::MIN_MAG_MIP_LINEAR;
            Samplers::Shadow->BorderColor = glm::vec4(1, 1, 1, 1);
            Samplers::Shadow->AddressU    = sSampler::eAddress::BORDER;
            Samplers::Shadow->AddressV    = sSampler::eAddress::BORDER;
            Samplers::Shadow->AddressW    = sSampler::eAddress::BORDER;
            Samplers::Shadow->Init();

            Viewports::Shadow = new sViewport();
            Viewports::Shadow->Width = 400;
            Viewports::Shadow->Height = 300;

            sTexture* shadowColor = new sTexture();
            shadowColor->Width = Viewports::Shadow->Width;
            shadowColor->Height = Viewports::Shadow->Height;
            shadowColor->Format = eTextureFormat::R32;
            shadowColor->InitializeData = false;
            shadowColor->EnableRenderTarget = true;
            shadowColor->Slot = K_SLOT_SHADOW_ATLAS;
            shadowColor->SetResizable(true);
            shadowColor->Init();

            sTexture* shadowDepth = new sTexture();
            shadowDepth->Type = sTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            shadowDepth->Width = Viewports::Shadow->Width;
            shadowDepth->Height = Viewports::Shadow->Height;
            shadowDepth->Format = eTextureFormat::R32_TYPELESS;
            shadowDepth->InitializeData = false;
            shadowDepth->EnableRenderTarget = true;
            shadowDepth->Slot = K_SLOT_SHADOW_ATLAS;
            shadowDepth->SetResizable(true);
            shadowDepth->Init();

            RenderTargets::Shadow = new sRenderTarget({ shadowColor }, shadowDepth, Viewports::Shadow);
            RenderTargets::Shadow->SetResizable(true);
            RenderTargets::Shadow->ClearColors.emplace_back(glm::vec4(1.0f));
            RenderTargets::Shadow->ClearDepth = 1.0f;
        }

        void cShadowManager::Free()
        {
            delete Buffers::ShadowPCF;
            delete Samplers::Shadow;
            delete RenderTargets::Shadow;
            delete Viewports::Shadow;
            for (auto* color : RenderTargets::Shadow->Colors) {
                delete color;
            }
            delete RenderTargets::Shadow->DepthStencil;
        }

        void cShadowManager::Flush()
        {
            Buffers::ShadowPCF->Flush();
        }

    }

}
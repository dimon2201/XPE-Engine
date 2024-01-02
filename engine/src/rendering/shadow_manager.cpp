#include <rendering/shadow_manager.hpp>
#include <rendering/bindings.hpp>

namespace xpe {

    namespace render {

        void cShadowManager::Init()
        {
            Buffers::ShadowPCF = new cShadowPcfBuffer();

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

            cTexture* shadowColor = new cTexture();
            shadowColor->SetWidth(Viewports::Shadow->Width);
            shadowColor->SetHeight(Viewports::Shadow->Height);
            shadowColor->SetFormat(eTextureFormat::R32);
            shadowColor->SetInitializeData(false);
            shadowColor->SetEnableRenderTarget(true);
            shadowColor->SetSlot(K_SLOT_SHADOW_ATLAS);
            shadowColor->SetResizable(true);
            shadowColor->Init();

            cTexture* shadowDepth = new cTexture();
            shadowDepth->SetType(cTexture::eType::TEXTURE_2D_DEPTH_STENCIL);
            shadowDepth->SetWidth(Viewports::Shadow->Width);
            shadowDepth->SetHeight(Viewports::Shadow->Height);
            shadowDepth->SetFormat(eTextureFormat::R32_TYPELESS);
            shadowDepth->SetInitializeData(false);
            shadowDepth->SetEnableRenderTarget(true);
            shadowDepth->SetSlot(K_SLOT_SHADOW_ATLAS);
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
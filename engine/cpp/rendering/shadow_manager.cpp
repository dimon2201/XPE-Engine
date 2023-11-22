#include <rendering/shadow_manager.h>

namespace xpe {

    namespace render {

        sSampler cShadowManager::s_Sampler;
        sShadowPcfBuffer* cShadowManager::s_Buffer = nullptr;

        void cShadowManager::Init()
        {
            s_Buffer = new sShadowPcfBuffer();
            s_Sampler.Slot        = K_SLOT_SHADOW_SAMPLER;
            s_Sampler.Filter      = sSampler::eFilter::MIN_MAG_MIP_LINEAR;
            s_Sampler.BorderColor = glm::vec4(1, 1, 1, 1);
            s_Sampler.AddressU    = sSampler::eAddress::BORDER;
            s_Sampler.AddressV    = sSampler::eAddress::BORDER;
            s_Sampler.AddressW    = sSampler::eAddress::BORDER;
            context::CreateSampler(s_Sampler);
        }

        void cShadowManager::Free()
        {
            delete s_Buffer;
            context::FreeSampler(s_Sampler);
        }

        void cShadowManager::Flush()
        {
            s_Buffer->Flush();
        }

        sSampler* cShadowManager::GetSampler()
        {
            return &s_Sampler;
        }

        sShadowPcfBuffer* cShadowManager::GetBuffer()
        {
            return s_Buffer;
        }

        sShadowPcf& cShadowManager::GetData()
        {
            return s_Buffer->Item;
        }

    }

}
#include <rendering/light_manager.hpp>
#include <rendering/bindings.hpp>

namespace xpe {

    namespace render {

        usize cLightManager::m_MaxLightCount = 0;
        glm::vec2 cLightManager::m_AtlasSize = glm::vec2(0.0f);

        void cLightManager::Init(usize maxLightCount, const glm::vec2& shadowMapSize)
        {
            m_MaxLightCount = maxLightCount;

            // Create buffer
            Buffers::DirectLight = new cLightBuffer(maxLightCount);

            // Create render target
            //s32 sqrtVal = glm::ceil(glm::sqrt(maxLightCount) + 1.0f);
            //m_AtlasSize = glm::vec2(sqrtVal * shadowMapSize.x, sqrtVal * shadowMapSize.y);
            m_AtlasSize = shadowMapSize;

            Buffers::ShadowPCF = new cShadowPcfBuffer();

            Viewports::Shadow = new sViewport(glm::vec4(0.0f, 0.0f, shadowMapSize.x, shadowMapSize.y), glm::vec2(0.0f, 1.0f));

            cTexture* shadowDepth = new cTexture(
                cTexture::eType::TEXTURE_2D_DEPTH_STENCIL,
                cResource::eViewType::SRV,
                cTexture::eUsage::DEFAULT,
                glm::vec3(m_AtlasSize.x, m_AtlasSize.y, 1.0f),
                1,
                eTextureFormat::R16_TYPELESS,
                1,
                true,
                K_SLOT_SHADOW_ATLAS,
                0,
                false,
                {}
            );

            RenderTargets::Shadow = new cRenderTarget({}, shadowDepth);
            RenderTargets::Shadow->SetResizable(false);
            RenderTargets::Shadow->SetClearDepth(1.0f);
        }

        void cLightManager::Free()
        {
            delete Buffers::ShadowPCF;
            delete Viewports::Shadow;
            delete RenderTargets::Shadow->GetDepthStencil();
            delete RenderTargets::Shadow;
        }

    }

}
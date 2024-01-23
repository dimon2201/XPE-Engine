#include <rendering/light_manager.hpp>
#include <rendering/bindings.hpp>
#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        usize cLightManager::m_MaxShadowCasterCount = 0;
        glm::vec2 cLightManager::m_ShadowMapSize = glm::vec2(0.0f);
        glm::vec2 cLightManager::m_AtlasSize = glm::vec2(0.0f);
        cAtlas2D* cLightManager::m_Atlas = nullptr;

        void cLightManager::Init(usize maxShadowCasterCount, const glm::vec2& shadowMapSize)
        {
            m_MaxShadowCasterCount = maxShadowCasterCount;
            m_ShadowMapSize = shadowMapSize;

            // Create buffer
            Buffers::DirectLight = new cLightBuffer(maxShadowCasterCount);

            // Create render target
            s32 sqrtVal = glm::ceil(glm::sqrt(maxShadowCasterCount));
            m_AtlasSize = glm::vec2(sqrtVal * shadowMapSize.x, sqrtVal * shadowMapSize.y);

            Buffers::ShadowPCF = new cShadowPcfBuffer();

            Viewports::Shadow = new sViewport(glm::vec4(0.0f, 0.0f, m_AtlasSize.x, m_AtlasSize.y), glm::vec2(0.0f, 1.0f));

            m_Atlas = new cAtlas2D(
                cTexture::eType::TEXTURE_2D_DEPTH_STENCIL,
                cTexture::eUsage::DEFAULT,
                m_AtlasSize,
                1,
                eTextureFormat::R32_TYPELESS,
                1,
                true,
                K_SLOT_TEXTURE_SHADOW_ATLAS,
                0,
                false
            );

            RenderTargets::Shadow = new cRenderTarget({}, m_Atlas);
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

        sShadowCaster cLightManager::AddShadowCaster()
        {
            sAtlas2DTexture texture = m_Atlas->AddTexture(m_ShadowMapSize, nullptr, 0);

            return texture.Offsets;
        }

        void cLightManager::RemoveShadowCaster(const sShadowCaster& caster)
        {
            m_Atlas->RemoveTexture(caster.AtlasTexture);
        }

        void cLightManager::UpdateLights(cScene* scene)
        {
            auto components = scene->GetComponents<CDirectionalLight>();
            for (auto [entity, light] : components.each())
            {
                auto transform = scene->GetComponent<CTransform>(entity);
                light.View = glm::lookAt(transform.Position, transform.Position + light.Direction, glm::vec3(0.0f, 1.0f, 0.0f));
                light.Projection = glm::ortho(
                    -20.0f, 20.0f,
                    -20.0f, 20.0f,
                    0.01f,
                    100.0f
                );

                sLightData lightData;
                lightData.Position = transform.Position;
                lightData.Color = light.Color;
                lightData.View = light.View;
                lightData.Projection = light.Projection;

                if (scene->HasAnyComponent<CShadowCaster>(entity))
                {
                    auto shadowCaster = scene->GetComponent<CShadowCaster>(entity);
                    lightData.ShadowCaster = shadowCaster.AtlasTexture.Offsets;
                }
                else
                {
                    lightData.ShadowCaster = glm::vec4(0.0f);
                }

                Buffers::DirectLight->GetList()[light.LightIndex] = lightData;
            }

            Buffers::DirectLight->Flush();
        }

    }

}
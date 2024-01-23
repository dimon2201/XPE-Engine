#pragma once

#include <rendering/core/core.hpp>

namespace xpe {

    namespace render {

        struct ENGINE_API sShadowCaster
        {
            sShadowCaster() = default;
            sShadowCaster(const glm::vec4& offsets)
            {
                AtlasTexture.Offsets = offsets;
            }

            sAtlas2DTexture AtlasTexture;
        };

        class ENGINE_API cLightManager final
        {

        public:
            static void Init(usize maxShadowCasterCount, const glm::vec2& shadowMapSize);
            static void Free();

            static sShadowCaster AddShadowCaster();
            static void RemoveShadowCaster(const sShadowCaster& light);
            static void UpdateLights(cScene* scene);

            static inline cAtlas2D* GetAtlas() { return m_Atlas; }

        private:
            static usize m_MaxShadowCasterCount;
            static glm::vec2 m_ShadowMapSize;
            static glm::vec2 m_AtlasSize;
            static cAtlas2D* m_Atlas;

        };

    }

}
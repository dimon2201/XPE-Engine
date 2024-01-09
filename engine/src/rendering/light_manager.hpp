#pragma once

#include <rendering/core/core.hpp>

namespace xpe {

    namespace render {

        class ENGINE_API cLightManager final
        {

        public:
            static void Init(usize maxLightCount, const glm::vec2& shadowMapSize);
            static void Free();

            static sAtlas2DTexture AddLight();

        private:
            static usize m_MaxLightCount;
            static glm::vec2 m_AtlasSize;

        };

    }

}
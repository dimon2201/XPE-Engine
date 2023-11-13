#pragma once

#include <rendering/buffers/widget_buffers.h>
#include <rendering/font/font.hpp>

namespace xpe {

    namespace render {

        using namespace math;
        using namespace res;

        struct ENGINE_API sWidgetAtlasCell final
        {
            u32 AtlasIndex;
        };

        class ENGINE_API cWidgetManager final
        {

        public:
            static void Init();
            static void Free();
            static void Clear();
            static void Bind(sPipeline& pipeline);

        private:
            static sTexture* s_TextureAtlas;
            static sSampler s_TextureSampler;
        };

    }

}
#pragma once

#include <rendering/buffers/shadow_filter_buffer.h>
#include <rendering/core/texture.h>

namespace xpe {

    namespace render {

        class ENGINE_API Shadow final
        {

        public:
            Shadow();
            ~Shadow();

            static void Init();
            static void Free();
            static Shadow& Get();

            ShadowFilterBuffer* GetBuffer();
            TextureSampler* GetSampler();
            Texture* GetMap();
            Texture* GetCoords();

            void SetFilter(const ShadowFilter& filter);

        private:
            static Shadow* s_Instance;
            ShadowFilterBuffer* m_FilterBuffer;
            TextureSampler* m_Sampler;
            Texture* m_Map;
            Texture* m_Coords;
        };

    }

}
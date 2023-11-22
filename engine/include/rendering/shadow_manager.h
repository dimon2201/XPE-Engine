#pragma once

#include <rendering/buffers/shadow_pcf_buffer.h>
#include <rendering/core/texture.h>

namespace xpe {

    namespace render {

        class ENGINE_API cShadowManager final
        {

        public:
            static void Init();
            static void Free();
            static void Flush();

            static sSampler* GetSampler();
            static sShadowPcfBuffer* GetBuffer();
            static sShadowPcf& GetData();

        private:
            static sSampler s_Sampler;
            static sShadowPcfBuffer* s_Buffer;

        };

    }

}
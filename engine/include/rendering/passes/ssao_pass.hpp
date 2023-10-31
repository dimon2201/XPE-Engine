#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/buffers/ssao_buffer.h>

namespace xpe
{
    namespace render
    {

        class ENGINE_API cSsaoPass : public cRenderPass
        {

        public:
            cSsaoPass(const vector<sRenderPassBinding>& bindings, sViewport* viewport, u32 sampleCount);

            inline sSsaoData& GetData() { return m_Buffer.Item; }
            inline void Flush() { m_Buffer.Flush(); }

        private:
            sSsaoBuffer m_Buffer;
        };

    }
}
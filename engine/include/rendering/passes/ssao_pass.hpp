#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/buffers/ssao_buffer.h>

namespace xpe
{
    namespace render
    {

        class ENGINE_API SSAOPass : public RenderPass
        {

        public:
            SSAOPass(const vector<RenderPassBinding>& bindings, Viewport* viewport, u32 sampleCount);

            inline SSAOData& GetData() { return m_Buffer.Item; }
            inline void Flush() { m_Buffer.Flush(); }

        private:
            SSAOBuffer m_Buffer;
        };

    }
}
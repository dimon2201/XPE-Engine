#pragma once

#include <rendering/passes/render_pass.h>

namespace xpe
{
    namespace render
    {
        struct SSAOBufferLayout
        {
            float SSAODirectionCount;
            float SSAOSampleCount;
            float _pad[2];
        };

        class ENGINE_API SSAOPass : public RenderPass
        {

        public:
            SSAOPass(const vector<RenderPassBinding>& bindings, RenderTarget* output);

            virtual void Draw(Scene* scene) override final;

        private:
            Buffer m_Buffer;
            SSAOBufferLayout m_BufferData;
            Ref<Geometry<Vertex2D>> m_Quad;
        };
    }
}
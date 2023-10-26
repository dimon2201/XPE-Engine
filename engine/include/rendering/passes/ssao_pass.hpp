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
            float SSAOIntensity = 1.0;
            float _pad[1];
        };

        class ENGINE_API SSAOPass : public RenderPass
        {

        public:
            SSAOPass(const vector<RenderPassBinding>& bindings, Viewport* viewport, u32 sampleCount);

            void Draw(Scene* scene) override final;

            void Update(Scene *scene) override;

            inline SSAOBufferLayout& GetData() { return m_BufferData; }

        private:
            Buffer m_Buffer;
            SSAOBufferLayout m_BufferData;
        };
    }
}
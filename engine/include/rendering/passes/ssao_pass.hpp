#pragma once

#include <rendering/core/core.h>
#include <rendering/passes/render_pass.h>

namespace xpe
{
    namespace render
    {
        struct Viewport;
        class GeometryStorage;

        struct SSAOBufferLayout
        {
            float SSAODirectionCount;
            float SSAOSampleCount;
            float _pad[2];
        };

        class ENGINE_API SSAOPass : public RenderPass
        {

        public:
            SSAOPass(const core::vector<RenderPassBinding>& bindings, Viewport* viewport, core::Boolean useMSAA, core::usize msaaSampleCount);
            ~SSAOPass();

            virtual void Update(Scene* scene) override final;
            virtual void Draw(Scene* scene) override final;

        private:
            Buffer m_Buffer;
            SSAOBufferLayout m_BufferData;

        };
    }
}
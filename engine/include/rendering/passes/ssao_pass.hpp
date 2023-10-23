#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/geometry/geometry.h>

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
            SSAOPass(const vector<RenderPassBinding>& bindings, Viewport* viewport);

            virtual void Draw(Scene* scene) override final;

            void Update(Scene *scene) override;

        private:
            Buffer m_Buffer;
            SSAOBufferLayout m_BufferData;
            Ref<Geometry> m_Quad;
        };
    }
}
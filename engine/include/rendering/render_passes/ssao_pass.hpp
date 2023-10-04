#pragma once

#include <rendering/render_passes/render_pass.h>
#include <geometry/geometries.h>

namespace xpe
{
    namespace render
    {
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
            SSAOPass(GeometryStorage* geometry, const core::vector<RenderPassBinding>& bindings, RenderTarget* output);
            ~SSAOPass();

            virtual void Update(Scene* scene) override final;
            virtual void Draw(Scene* scene) override final;

        private:
            Buffer m_Buffer;
            SSAOBufferLayout m_BufferData;
            Ref<GeometryIndexed<Vertex2D>> m_Quad;
        };
    }
}
#include "core/build.h"
#include "rendering/draw/render_pass.h"
#include "rendering/core/core.h"
#include "geometry/geometries.h"
#include "stl/vector.h"

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

        class ENGINE_API SSAORenderPass : public RenderPass
        {

        public:
            SSAORenderPass(GeometryStorage* geometry, const core::vector<RenderPassBinding>& bindings, RenderTarget* output);
            ~SSAORenderPass();

            virtual void Update(Scene* scene) override final;
            virtual void Draw(Scene* scene) override final;

        private:
            Buffer m_Buffer;
            SSAOBufferLayout m_BufferData;
            Ref<GeometryIndexed<Vertex2D>> m_Quad;

        };
    }
}
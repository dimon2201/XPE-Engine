#include "core/build.h"
#include "render_pass.h"
#include "rendering/core/core.h"
#include "geometry/geometries.h"
#include "stl/vector.h"

namespace xpe
{
    namespace render
    {
        class GeometryStorage;

        class ENGINE_API FXAAPass : public RenderPass
        {

        public:
            FXAAPass(
                const core::vector<RenderPassBinding>& bindings,
                const glm::vec2& canvasSize,
                Viewport* canvasViewport
            );
            ~FXAAPass();

            virtual void Update(Scene* scene) override final;
            virtual void Draw(Scene* scene) override final;

            inline RenderTarget* GetRenderTarget() { return m_Pipeline->RenderTarget; }

        private:
            TextureSampler m_Sampler;

        };
    }
}
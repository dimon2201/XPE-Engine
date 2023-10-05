#include "core/build.h"
#include "render_pass.h"
#include "rendering/core/viewport.h"
#include "rendering/core/texture.h"
#include "rendering/core/pipeline.h"
#include "stl/vector.h"

namespace xpe
{
    namespace render
    {
        class ENGINE_API FXAAPass : public RenderPass
        {

        public:
            FXAAPass(const core::vector<RenderPassBinding>& bindings, Viewport* viewport);
            ~FXAAPass();

            virtual void Update(Scene* scene) override final;
            virtual void Draw(Scene* scene) override final;

            inline RenderTarget* GetRenderTarget() { return m_Pipeline->RenderTarget; }

        private:
            TextureSampler m_Sampler;

        };
    }
}
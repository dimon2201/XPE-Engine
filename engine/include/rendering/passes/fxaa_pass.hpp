#include <rendering/passes/render_pass.h>

namespace xpe
{
    namespace render
    {
        class ENGINE_API FXAAPass : public RenderPass
        {

        public:
            FXAAPass(const core::vector<RenderPassBinding>& bindings, Viewport* viewport);
            ~FXAAPass();

        private:
            TextureSampler m_Sampler;

        };
    }
}
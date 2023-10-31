#include <rendering/passes/render_pass.h>

namespace xpe
{
    namespace render
    {
        class ENGINE_API cFxaaPass : public cRenderPass
        {

        public:
            cFxaaPass(const core::vector<sRenderPassBinding>& bindings, sViewport* viewport);
            ~cFxaaPass();

        private:
            sSampler m_Sampler;

        };
    }
}
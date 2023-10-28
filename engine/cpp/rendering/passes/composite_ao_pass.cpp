#include <rendering/passes/composite_ao_pass.h>

namespace xpe {

    namespace render {

        CompositeAOPass::CompositeAOPass(const vector<RenderPassBinding>& bindings)
        : RenderPass(eType::POSTFX, bindings) {}

    }

}
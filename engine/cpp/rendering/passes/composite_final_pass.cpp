#include <rendering/passes/composite_final_pass.h>

namespace xpe {

    namespace render {

        CompositeFinalPass::CompositeFinalPass(const vector<RenderPassBinding>& bindings)
        : RenderPass(eType::POSTFX, bindings) {}

    }

}
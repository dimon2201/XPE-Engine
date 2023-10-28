#include <rendering/passes/final_pass.h>

namespace xpe {

    namespace render {

        FinalPass::FinalPass(const vector<RenderPassBinding>& bindings)
        : RenderPass(eType::FINAL, bindings) {}

    }

}
#include <rendering/passes/final_pass.h>

namespace xpe {

    namespace render {

        cFinalPass::cFinalPass(const vector<sRenderPassBinding>& bindings)
        : cRenderPass(eType::FINAL, bindings) {}

    }

}
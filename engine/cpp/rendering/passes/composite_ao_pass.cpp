#include <rendering/passes/composite_ao_pass.h>

namespace xpe {

    namespace render {

        cCompositeAoPass::cCompositeAoPass(const vector<sRenderPassBinding>& bindings)
        : cRenderPass(eType::POSTFX, bindings) {}

    }

}
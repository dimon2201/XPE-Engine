#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/geometry/geometry.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace ecs;
        using namespace math;

        class ENGINE_API cSkyboxPass : public cRenderPass
        {

        public:
            cSkyboxPass(const vector<sRenderPassBinding>& bindings);
            ~cSkyboxPass() override;

            void DrawOpaque(cScene *scene) override;

        protected:
            void InitOpaque() override;

        protected:
            sSampler m_Sampler;
        };

    }

}
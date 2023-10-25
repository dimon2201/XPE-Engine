#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/geometry/geometry.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace ecs;
        using namespace math;

        class ENGINE_API SkyboxPass : public RenderPass
        {

        public:
            SkyboxPass(eType type, const vector<RenderPassBinding>& bindings);
            ~SkyboxPass() override;

            void DrawOpaque(Scene *scene) override;

        protected:
            TextureSampler m_Sampler;
            Ref<Geometry> m_Cube;
        };

    }

}
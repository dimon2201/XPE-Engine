#pragma once

#include <rendering/passes/render_pass.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace ecs;
        using namespace math;

        class ENGINE_API SkyboxPass : public RenderPass
        {

        public:
            SkyboxPass(
                    const vector<RenderPassBinding>& bindings,
                    RenderTarget* output
            );
            ~SkyboxPass() override;

            virtual void Draw(Scene* scene) override;

        protected:
            TextureSampler m_Sampler;
            Ref<Geometry<Vertex3D>> m_Cube;
        };

    }

}
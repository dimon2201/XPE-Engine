#pragma once

#include <rendering/passes/render_pass.h>
#include <geometry/geometries.h>

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        using namespace ecs;

        class GeometryStorage;

        class ENGINE_API MergePass : public RenderPass
        {

        public:
            MergePass(
                const core::vector<RenderPassBinding>& bindings,
                RenderTarget* output
            );
            virtual ~MergePass();

            virtual void Update(Scene* scene) override;
            virtual void Draw(Scene* scene) override;

        };

    }

}
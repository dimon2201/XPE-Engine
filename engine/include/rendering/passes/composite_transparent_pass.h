#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/geometry/geometries.h>

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        using namespace ecs;

        class GeometryStorage;

        class ENGINE_API CompositeTransparentPass : public RenderPass
        {

        public:
            CompositeTransparentPass(
                const core::vector<RenderPassBinding>& bindings
            );
            virtual ~CompositeTransparentPass();

            virtual void Draw(Scene* scene) override;

        };

    }

}
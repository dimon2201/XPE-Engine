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

        class ENGINE_API CompositeFinalPass : public RenderPass
        {

        public:
            CompositeFinalPass(
                const core::vector<RenderPassBinding>& bindings
            );
            virtual ~CompositeFinalPass();

            virtual void Draw(Scene* scene) override;

        };

    }

}
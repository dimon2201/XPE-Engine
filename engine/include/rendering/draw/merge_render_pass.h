#pragma once

#include <rendering/draw/render_pass.h>
#include <geometry/geometries.h>

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        using namespace ecs;

        class GeometryStorage;

        class ENGINE_API MergeRenderPass : public RenderPass
        {

        public:
            MergeRenderPass(
                const core::vector<RenderPassBinding>& bindings,
                RenderTarget* output,
                GeometryStorage* geometryStorage
            );
            virtual ~MergeRenderPass();

            virtual void Update(Scene* scene) override;
            virtual void Draw(Scene* scene) override;

        };

    }

}
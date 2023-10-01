#pragma once

#include <rendering/passes/render_pass.h>

#include <rendering/buffers/vertex_buffer.h>
#include <rendering/buffers/index_buffer.h>

#include <geometry/vertices.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace ecs;
        using namespace math;

        class GeometryStorage;

        class ENGINE_API SkyboxPass : public RenderPass
        {

        public:
            SkyboxPass(
                const core::vector<RenderPassBinding>& bindings,
                RenderTarget* output,
                GeometryStorage* geometryStorage
            );
            ~SkyboxPass() override;

            virtual void Update(Scene* scene) override;
            virtual void Draw(Scene* scene) override;

        protected:
            TextureSampler m_Sampler;
        };

    }

}
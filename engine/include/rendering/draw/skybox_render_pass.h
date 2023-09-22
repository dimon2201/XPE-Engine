#pragma once

#include <rendering/draw/render_pass.h>

#include <rendering/buffers/vertex_buffer.h>
#include <rendering/buffers/index_buffer.h>

#include <geometry/vertices.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace ecs;
        using namespace math;

        class GeometryStorage;

        class ENGINE_API SkyboxRenderPass : public RenderPass
        {

        public:
            SkyboxRenderPass(
                const core::vector<RenderPassBinding>& bindings,
                RenderTarget* output,
                GeometryStorage* geometryStorage
            );
            ~SkyboxRenderPass() override;

            virtual void Update(Scene* scene) override;
            virtual void Draw(Scene* scene) override;

        protected:
            TextureSampler m_Sampler;
        };

    }

}
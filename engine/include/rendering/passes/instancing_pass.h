#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/buffers/instance_buffer.h>
#include <rendering/buffers/transform_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace math;
        using namespace ecs;

        class ENGINE_API InstancingPass : public RenderPass
        {

        public:
            InstancingPass(eType type, const vector<RenderPassBinding>& bindings);

        protected:
            void DrawInstanced(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                Entity* entity,
                const vector<Entity*>& entities
            );

            void DrawSingle(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                Entity* entity
            );

            void DrawMultiple(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                const vector<Entity*>& entities
            );

            InstanceBuffer m_InstanceBuffer;
            TransformBuffer m_TransformBuffer;
        };

    }

}
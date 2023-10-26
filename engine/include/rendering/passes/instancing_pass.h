#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/buffers/instance_buffer.h>
#include <rendering/buffers/transform_buffer.h>

namespace xpe {

    namespace render {

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
                const vector<Entity*>& entities,
                const std::function<void(Entity* entity, RenderInstance&)>& callback = {},
                const glm::mat4x4& lightMatrix = {}
            );

            void DrawSingle(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                Entity* entity,
                const std::function<void(Entity* entity, RenderInstance&)>& callback = {},
                const glm::mat4x4& lightMatrix = {}
            );

            void DrawMultiple(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                const vector<Entity*>& entities,
                const std::function<void(Entity* entity, RenderInstance&)>& callback = {},
                const glm::mat4x4& lightMatrix = {}
            );

            InstanceBuffer m_InstanceBuffer;
            TransformBuffer m_TransformBuffer;
        };

    }

}
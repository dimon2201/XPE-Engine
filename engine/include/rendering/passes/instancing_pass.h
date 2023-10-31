#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/buffers/instance_buffer.h>
#include <rendering/buffers/transform_buffer.h>

namespace xpe {

    namespace render {

        class ENGINE_API cInstancingPass : public cRenderPass
        {

        public:
            cInstancingPass(eType type, const vector<sRenderPassBinding>& bindings);

        protected:
            void DrawInstanced(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                cEntity* entity,
                const vector<cEntity*>& entities,
                const std::function<void(cEntity* entity, sRenderInstance&)>& callback = {},
                const glm::mat4x4& lightMatrix = {}
            );

            void DrawSingle(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                cEntity* entity,
                const std::function<void(cEntity* entity, sRenderInstance&)>& callback = {},
                const glm::mat4x4& lightMatrix = {}
            );

            void DrawMultiple(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                const vector<cEntity*>& entities,
                const std::function<void(cEntity* entity, sRenderInstance&)>& callback = {},
                const glm::mat4x4& lightMatrix = {}
            );

            sInstanceBuffer m_InstanceBuffer;
            sTransformBuffer m_TransformBuffer;
        };

    }

}
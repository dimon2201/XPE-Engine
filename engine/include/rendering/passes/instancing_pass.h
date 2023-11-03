#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/buffers/instance_buffer.h>

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
                const std::function<void(cEntity* entity, sRenderInstance&)>& callback = {}
            );

            void DrawSingle(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                cEntity* entity,
                const std::function<void(cEntity* entity, sRenderInstance&)>& callback = {}
            );

            void DrawMultiple(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                const vector<cEntity*>& entities,
                const std::function<void(cEntity* entity, sRenderInstance&)>& callback = {}
            );

            sInstanceBuffer m_InstanceBuffer;
        };

    }

}
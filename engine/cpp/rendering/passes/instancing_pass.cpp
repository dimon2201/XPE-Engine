#include <rendering/passes/instancing_pass.h>

namespace xpe {

    namespace render {

        cInstancingPass::cInstancingPass(
            eType type,
            const vector<sRenderPassBinding>& bindings
        ) : cRenderPass(type, bindings)
        {
            m_InstanceBuffer.Reserve(1000);
        }

        void cInstancingPass::DrawInstanced(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                cEntity* entity,
                const vector<cEntity*>& entities,
                const std::function<void(cEntity* entity, sRenderInstance&)>& callback
        ) {
            if (entities.empty()) {
                DrawSingle(primitiveTopology, vertexOffset, vertexCount, indexOffset, indexCount, entity, callback);
            } else {
                DrawMultiple(primitiveTopology, vertexOffset, vertexCount, indexOffset, indexCount, entities, callback);
            }
        }

        void cInstancingPass::DrawSingle(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                cEntity* entity,
                const std::function<void(cEntity* entity, sRenderInstance&)>& callback
        ) {
            if (!entity->Visible)
                return;

            m_InstanceBuffer.Clear();

            sRenderInstance instance;
            instance.ModelMatrix = MathManager::UpdateModelMatrix(entity->GetTransform());
            instance.NormalMatrix = MathManager::UpdateNormalMatrix(instance.ModelMatrix);
            if (callback) {
                callback(entity, instance);
            }

            m_InstanceBuffer.Add(instance);
            m_InstanceBuffer.Flush();

            context::BindPrimitiveTopology(primitiveTopology);
            context::BindVSBuffer(m_InstanceBuffer);

            if (indexCount == 0) {
                context::DrawVertexed(vertexCount, 1, vertexOffset);
            } else {
                context::DrawIndexed(indexCount, 1, vertexOffset, indexOffset);
            }

            context::UnbindVSBuffer(m_InstanceBuffer);
        }

        void cInstancingPass::DrawMultiple(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                const vector<cEntity*>& entities,
                const std::function<void(cEntity* entity, sRenderInstance&)>& callback
        ) {
            usize entityCount = entities.size();
            usize instanceCount = 0;
            m_InstanceBuffer.Clear();
            for (usize i = 0 ; i < entityCount ; i++)
            {
                auto& entity = entities[i];

                if (!entity->Visible)
                    continue;

                sRenderInstance instance;
                instance.ModelMatrix = MathManager::UpdateModelMatrix(entity->GetTransform());
                instance.NormalMatrix = MathManager::UpdateNormalMatrix(instance.ModelMatrix);
                if (callback) {
                    callback(entity, instance);
                }

                m_InstanceBuffer.Add(instance);

                instanceCount++;
            }
            m_InstanceBuffer.Flush();

            context::BindPrimitiveTopology(primitiveTopology);
            context::BindVSBuffer(m_InstanceBuffer);

            if (indexCount == 0) {
                context::DrawVertexed(vertexCount, instanceCount, vertexOffset);
            } else {
                context::DrawIndexed(indexCount, instanceCount, vertexOffset, indexOffset);
            }

            context::UnbindVSBuffer(m_InstanceBuffer);
        }

    }

}
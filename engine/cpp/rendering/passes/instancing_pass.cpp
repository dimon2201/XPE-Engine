#include <rendering/passes/instancing_pass.h>

namespace xpe {

    namespace render {

        cInstancingPass::cInstancingPass(
            eType type,
            const vector<sRenderPassBinding>& bindings
        ) : cRenderPass(type, bindings)
        {
            m_InstanceBuffer.Reserve(1000);
            m_TransformBuffer.Reserve(1000);
        }

        void cInstancingPass::DrawInstanced(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                cEntity* entity,
                const vector<cEntity*>& entities,
                const std::function<void(cEntity* entity, sRenderInstance&)>& callback,
                const glm::mat4x4& lightMatrix
        ) {
            if (entities.empty()) {
                DrawSingle(primitiveTopology, vertexOffset, vertexCount, indexOffset, indexCount, entity, callback, lightMatrix);
            } else {
                DrawMultiple(primitiveTopology, vertexOffset, vertexCount, indexOffset, indexCount, entities, callback, lightMatrix);
            }
        }

        void cInstancingPass::DrawSingle(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                cEntity* entity,
                const std::function<void(cEntity* entity, sRenderInstance&)>& callback,
                const glm::mat4x4& lightMatrix
        ) {
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();

            sRenderInstance instance;
            instance.TransformIndex = 0;
            if (callback) {
                callback(entity, instance);
            }

            m_InstanceBuffer.Add(instance);
            m_TransformBuffer.AddTransform(entity->GetTransform(), lightMatrix);

            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            context::BindPrimitiveTopology(primitiveTopology);
            context::BindVSBuffer(m_InstanceBuffer);
            context::BindVSBuffer(m_TransformBuffer);

            if (indexCount == 0) {
                context::DrawVertexed(vertexCount, 1, vertexOffset);
            } else {
                context::DrawIndexed(indexCount, 1, vertexOffset, indexOffset);
            }

            context::UnbindVSBuffer(m_InstanceBuffer);
            context::UnbindVSBuffer(m_TransformBuffer);
        }

        void cInstancingPass::DrawMultiple(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                const vector<cEntity*>& entities,
                const std::function<void(cEntity* entity, sRenderInstance&)>& callback,
                const glm::mat4x4& lightMatrix
        ) {
            usize entityCount = entities.size();
            usize instanceCount = 0;
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();
            for (usize i = 0 ; i < entityCount ; i++)
            {
                auto& entity = entities[i];

                sRenderInstance instance;
                instance.TransformIndex = i;
                if (callback) {
                    callback(entity, instance);
                }

                m_InstanceBuffer.Add(instance);
                m_TransformBuffer.AddTransform(entity->GetTransform(), lightMatrix);

                instanceCount++;
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            context::BindPrimitiveTopology(primitiveTopology);
            context::BindVSBuffer(m_InstanceBuffer);
            context::BindVSBuffer(m_TransformBuffer);

            if (indexCount == 0) {
                context::DrawVertexed(vertexCount, instanceCount, vertexOffset);
            } else {
                context::DrawIndexed(indexCount, instanceCount, vertexOffset, indexOffset);
            }

            context::UnbindVSBuffer(m_InstanceBuffer);
            context::UnbindVSBuffer(m_TransformBuffer);
        }

    }

}
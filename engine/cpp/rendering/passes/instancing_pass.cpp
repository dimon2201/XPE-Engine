#include <rendering/passes/instancing_pass.h>

namespace xpe {

    namespace render {

        InstancingPass::InstancingPass(
            eType type,
            const vector<RenderPassBinding>& bindings
        ) : RenderPass(type, bindings)
        {
            m_InstanceBuffer.Reserve(1000);
            m_TransformBuffer.Reserve(1000);
        }

        void InstancingPass::DrawInstanced(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                Entity* entity,
                const vector<Entity*>& entities,
                const std::function<void(Entity* entity, RenderInstance&)>& callback,
                const glm::mat4x4& lightMatrix
        ) {
            if (entities.empty()) {
                DrawSingle(primitiveTopology, vertexOffset, vertexCount, indexOffset, indexCount, entity, callback, lightMatrix);
            } else {
                DrawMultiple(primitiveTopology, vertexOffset, vertexCount, indexOffset, indexCount, entities, callback, lightMatrix);
            }
        }

        void InstancingPass::DrawSingle(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                Entity* entity,
                const std::function<void(Entity* entity, RenderInstance&)>& callback,
                const glm::mat4x4& lightMatrix
        ) {
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();

            RenderInstance instance;
            instance.TransformIndex = 0;
            if (callback) {
                callback(entity, instance);
            }

            m_InstanceBuffer.Add(instance);
            m_TransformBuffer.AddTransform(entity->Transform, lightMatrix);

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

        void InstancingPass::DrawMultiple(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                const vector<Entity*>& entities,
                const std::function<void(Entity* entity, RenderInstance&)>& callback,
                const glm::mat4x4& lightMatrix
        ) {
            usize entityCount = entities.size();
            usize instanceCount = 0;
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();
            for (usize i = 0 ; i < entityCount ; i++)
            {
                auto& entity = entities[i];

                RenderInstance instance;
                instance.TransformIndex = i;
                if (callback) {
                    callback(entity, instance);
                }

                m_InstanceBuffer.Add(instance);
                m_TransformBuffer.AddTransform(entity->Transform, lightMatrix);

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
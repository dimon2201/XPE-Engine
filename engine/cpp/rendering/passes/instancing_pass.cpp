#include <rendering/passes/instancing_pass.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        InstancingPass::InstancingPass(
            const vector<RenderPassBinding>& bindings,
            RenderTarget* output,
            MaterialStorage* materialStorage
        ) : RenderPass(bindings, output)
        {
            m_InstanceBuffer.Reserve(1000);
            m_TransformBuffer.Reserve(1000);
            materialStorage->BindPipeline(*m_Pipeline);
        }

        InstancingPass::~InstancingPass() {}

        void InstancingPass::DrawInstanced(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                Entity* entity,
                const vector<Entity*>& entities
        ) {
            if (entities.empty()) {
                DrawSingle(primitiveTopology, vertexOffset, vertexCount, indexOffset, indexCount, entity);
            } else {
                DrawMultiple(primitiveTopology, vertexOffset, vertexCount, indexOffset, indexCount, entities);
            }
        }

        void InstancingPass::DrawSingle(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                Entity* entity
        ) {
            auto* materialComponent = entity->GetComponent<MaterialComponent>(entity->GetTag());
            if (materialComponent == nullptr) {
                LogInfo("if (materialComponent == nullptr) ");
                return;
            }

            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();

            RenderInstance instance;
            instance.CameraIndex = 0;
            instance.TransformIndex = 0;
            instance.MaterialIndex = materialComponent->Material->Index;

            m_InstanceBuffer.Add(instance);
            m_TransformBuffer.AddTransform(entity->Transform);

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
                const vector<Entity*>& entities
        ) {
            usize entityCount = entities.size();
            usize instanceCount = 0;
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();
            for (usize i = 0 ; i < entityCount ; i++)
            {
                auto& entity = entities[i];

                auto* materialComponent = entity->GetComponent<MaterialComponent>(entity->GetTag());
                if (materialComponent == nullptr) {
                    continue;
                }

                RenderInstance instance;
                instance.CameraIndex = 0;
                instance.TransformIndex = i;
                instance.MaterialIndex = materialComponent->Material->Index;

                m_InstanceBuffer.Add(instance);
                m_TransformBuffer.AddTransform(entity->Transform);

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
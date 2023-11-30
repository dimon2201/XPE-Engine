#include <rendering/passes/instancing_pass.h>
#include <ecs/components.hpp>

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
                cScene* scene,
                const sGeometryInfo& geometryInfo,
                const std::function<void(EntityID entityId, sRenderInstance&)>& callback
        ) {
            usize entityCount = geometryInfo.Entities.size();
            usize instanceCount = 0;
            m_InstanceBuffer.Clear();
            for (usize i = 0 ; i < entityCount ; i++)
            {
                auto& entity = geometryInfo.Entities[i];

                if (!scene->HasAnyComponent<CVisible>(entity))
                    continue;

                sRenderInstance instance;
                instance.ModelMatrix = cMathManager::UpdateModelMatrix(scene->GetComponent<CTransform>(entity));
                instance.NormalMatrix = cMathManager::UpdateNormalMatrix(instance.ModelMatrix);
                callback(entity, instance);

                m_InstanceBuffer.Add(instance);

                instanceCount++;
            }
            m_InstanceBuffer.Flush();

            context::BindPrimitiveTopology(geometryInfo.PrimitiveTopology);
            context::VSBindBuffer(m_InstanceBuffer);

            if (geometryInfo.IndexCount == 0) {
                context::DrawVertexed(geometryInfo.VertexCount, instanceCount, geometryInfo.VertexOffset);
            } else {
                context::DrawIndexed(geometryInfo.IndexCount, instanceCount, geometryInfo.VertexOffset, geometryInfo.IndexOffset);
            }

            context::VSUnbindBuffer(m_InstanceBuffer);
        }

    }

}
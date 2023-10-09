#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/buffers/instance_buffer.h>
#include <rendering/buffers/transform_buffer.h>
#include <rendering/storages/material_storage.h>

namespace xpe {

    namespace ecs
    {
        class Scene;
        class Entity;
    }

    namespace render {

        using namespace core;
        using namespace math;
        using namespace ecs;

        class ENGINE_API InstancingPass : public RenderPass
        {

        public:
            InstancingPass(
                const vector<RenderPassBinding>& bindings,
                RenderTarget* output,
                MaterialStorage* materialStorage
            );

            ~InstancingPass() override;

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
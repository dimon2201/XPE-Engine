#pragma once

#include <rendering/passes/instancing_pass.h>

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

        class ENGINE_API CollisionPass : public InstancingPass
        {

        public:
            CollisionPass(
                const vector<RenderPassBinding>& bindings,
                RenderTarget* output,
                MaterialStorage* materialStorage
            );

            ~CollisionPass();

            virtual void Update(Scene* scene) override final;
            virtual void Draw(Scene* scene) override final;

            void DrawInstanced(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                Transform transform
            );

            void DrawSingle(
                ePrimitiveTopology primitiveTopology,
                usize vertexOffset,
                usize vertexCount,
                usize indexOffset,
                usize indexCount,
                Transform transform
            );

        };

    }

}
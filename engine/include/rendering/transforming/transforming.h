#pragma once

#include <rendering/buffers/structure_buffer.h>
#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        using namespace ecs;

        struct ENGINE_API TransformData final {
            glm::mat4 ModelMatrix;
            glm::mat4 NormalMatrix;
        };

        class ENGINE_API TransformBuffer : public StructureBuffer<TransformData> {

        public:
            TransformBuffer() = default;

            TransformBuffer(usize size)
            : StructureBuffer<TransformData>(size, K_SLOT_TRANSFORMS, K_FALSE) {}

        };

        struct ENGINE_API Transform2DData final {
            glm::mat3 ModelMatrix;
        };

        class ENGINE_API Transform2DBuffer : public StructureBuffer<Transform2DData> {

        public:
            Transform2DBuffer() = default;

            Transform2DBuffer(usize size)
            : StructureBuffer<Transform2DData>(size, K_SLOT_TRANSFORMS2D, K_FALSE) {}

        };

        struct ENGINE_API TransformStorage : public Object {
            static const usize K_TRANSFORMS_SIZE = 1000;
            static const usize K_TRANSFORMS2D_SIZE = 1000;

            TransformBuffer Buffer;
            Transform2DBuffer Buffer2D;

            TransformStorage();
            ~TransformStorage();
        };

        class ENGINE_API TransformManager final {

        public:
            static void Init();
            static void Free();

            static TransformBuffer* GetBuffer();
            static Transform2DBuffer* GetBuffer2D();

            static void AddTransform(TransformComponent& transformComponent);
            static void AddTransform2D(Transform2DComponent& transformComponent);

            static void RemoveTransform(TransformComponent& transformComponent);
            static void RemoveTransform2D(Transform2DComponent& transformComponent);

            static void FlushTransform(const TransformComponent& transformComponent);
            static void FlushTransform2D(const Transform2DComponent& transform2DComponent);

            static void FlushTransforms();
            static void FlushTransforms2D();

            static void ClearTransforms();
            static void ClearTransforms2D();

        private:
            static TransformStorage* s_Storage;

        };

    }

}
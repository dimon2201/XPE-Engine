#pragma once

#include <rendering/structure_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API TransformData final {
            glm::mat4 Matrix;
        };

        class ENGINE_API TransformBuffer : public StructureBuffer<TransformData> {

        public:
            TransformBuffer() = default;

            TransformBuffer(Context* context, usize size)
            : StructureBuffer<TransformData>(context, size, K_SLOT_TRANSFORMS, K_FALSE) {}

        };

        struct ENGINE_API Transform2DData final {
            glm::mat3 Matrix;
        };

        class ENGINE_API Transform2DBuffer : public StructureBuffer<Transform2DData> {

        public:
            Transform2DBuffer() = default;

            Transform2DBuffer(Context* context, usize size)
            : StructureBuffer<Transform2DData>(context, size, K_SLOT_TRANSFORMS2D, K_FALSE) {}

        };

        class ENGINE_API TransformManager final {

        public:
            static const usize K_TRANSFORMS_SIZE = 1000000;
            static const usize K_TRANSFORMS2D_SIZE = 1000000;

        public:
            static void Init(Context* context);
            static void Free();

            static TransformBuffer* GetBuffer();
            static Transform2DBuffer* GetBuffer2D();

            static void UpdateTransform(u32 index, const cTransformComponent& transformComponent);
            static void UpdateTransform2D(u32 index, const cTransform2DComponent& transform2DComponent);

        private:
            static Context* s_Context;
            static TransformBuffer s_TransformBuffer;
            static Transform2DBuffer s_Transform2DBuffer;

        };

    }

}
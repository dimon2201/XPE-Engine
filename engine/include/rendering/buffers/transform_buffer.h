#pragma once

#include <rendering/buffers/structure_buffer.h>

#include <math/transform.h>

namespace xpe {

    namespace render {

        using namespace math;

        struct ENGINE_API TransformData final
        {
            glm::mat4 ModelMatrix;  // position world space
            glm::mat4 NormalMatrix; // normal world space
            glm::mat4 DLSMatrix;    // direct light space
        };

        struct ENGINE_API TransformBuffer : public StructureBuffer<TransformData>
        {

            TransformBuffer(usize size = 0) : StructureBuffer<TransformData>(size, K_SLOT_TRANSFORMS) {}

            void AddTransform(const Transform& transform);

        };

        struct ENGINE_API Transform2DData final
        {
            glm::mat3 ModelMatrix;
        };

        struct ENGINE_API Transform2DBuffer : public StructureBuffer<Transform2DData>
        {

            Transform2DBuffer(usize size = 0) : StructureBuffer<Transform2DData>(size, K_SLOT_TRANSFORMS2D) {}

            void AddTransform(const Transform2D& transform);

        };

    }

}
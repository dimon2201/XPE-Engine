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
            glm::mat4 LightMatrix;  // light space
        };

        struct ENGINE_API TransformBuffer : public StructureBuffer<TransformData>
        {
            TransformBuffer(usize size = 0) : StructureBuffer<TransformData>(size, K_SLOT_TRANSFORMS) {}

            void AddTransform(const Transform& transform, const glm::mat4x4& lightMatrix = {});
        };

    }

}
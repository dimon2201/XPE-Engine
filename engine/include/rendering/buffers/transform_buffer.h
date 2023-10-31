#pragma once

#include <rendering/buffers/structure_buffer.h>

#include <math/transform.h>

namespace xpe {

    namespace render {

        using namespace math;

        struct ENGINE_API sTransformData final
        {
            glm::mat4 ModelMatrix;  // position world space
            glm::mat4 NormalMatrix; // normal world space
            glm::mat4 LightMatrix;  // light space
        };

        struct ENGINE_API sTransformBuffer : public sListBuffer<sTransformData>
        {
            sTransformBuffer(usize size = 0) : sListBuffer<sTransformData>(size, K_SLOT_TRANSFORMS) {}
            void AddTransform(const sTransform& transform, const glm::mat4x4& lightMatrix = {});
        };

    }

}
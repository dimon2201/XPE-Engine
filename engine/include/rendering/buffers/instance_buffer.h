#pragma once

#include <rendering/buffers/list_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sRenderInstance final
        {
            u32 TransformIndex = 0;
            u32 MaterialIndex = 0;
            u32 SkeletonIndex = 0;
            glm::mat4 ModelMatrix;  // position world space
            glm::mat4 NormalMatrix; // normal world space
            glm::mat4 LightMatrix;  // light space
        };

        struct ENGINE_API sInstanceBuffer : public sListBuffer<sRenderInstance>
        {
            sInstanceBuffer(usize count = 0) : sListBuffer<sRenderInstance>(count, K_SLOT_INSTANCES) {}
        };

    }

}
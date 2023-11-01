#pragma once

#include <rendering/buffers/list_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API sSkeletonData final
        {
            glm::mat4 Transform;
        };

        struct ENGINE_API sSkeletonBuffer : public sListBuffer<sSkeletonData>
        {
            sSkeletonBuffer(usize size = 0) : sListBuffer<sSkeletonData>(size, K_SLOT_SKELETONS) {}
        };

    }

}
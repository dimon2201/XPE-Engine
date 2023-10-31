#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API sBoneData final
        {
            glm::mat4 Transform;
        };

        struct ENGINE_API sBoneBuffer : public sListBuffer<sBoneData>
        {
            sBoneBuffer(usize size = 0) : sListBuffer<sBoneData>(size, K_SLOT_BONES) {}
        };

    }

}
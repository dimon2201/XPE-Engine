#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API BoneData final
        {
            glm::mat4 Transform;
        };

        struct ENGINE_API BoneBuffer : public StructureBuffer<BoneData>
        {
            BoneBuffer(usize size = 0) : StructureBuffer<BoneData>(size, K_SLOT_BONES) {}
        };

    }

}
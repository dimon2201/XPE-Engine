#pragma once

#include <rendering/buffers/item_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API CameraData : public Object
        {
            glm::vec3 Position;
            glm::mat4 View;
            glm::mat4 Projection;
        };

        struct ENGINE_API CameraBuffer : public ItemBuffer<CameraData>
        {
            CameraBuffer() : ItemBuffer<CameraData>(K_SLOT_CAMERA) {}
        };

    }

}
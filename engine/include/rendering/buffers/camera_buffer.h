#pragma once

#include <rendering/buffers/item_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API CameraData final
        {
            glm::vec3 Position;
            glm::mat4 View;
            glm::mat4 Projection;
            float Gamma = 2.2f;
            float Exposure = 1.0f;
        };

        struct ENGINE_API CameraBuffer : public ItemBuffer<CameraData>
        {
            CameraBuffer() : ItemBuffer<CameraData>(K_SLOT_CAMERA) {}
        };

    }

}
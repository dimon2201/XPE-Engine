#pragma once

#include <rendering/buffers/item_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sCameraData : public cObject
        {
            glm::vec3 Position;
            glm::mat4 View;
            glm::mat4 Projection;
        };

        struct ENGINE_API sCameraBuffer : public sItemBuffer<sCameraData>
        {
            sCameraBuffer() : sItemBuffer<sCameraData>(K_SLOT_CAMERA) {}
        };

    }

}
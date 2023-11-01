#pragma once

#include <rendering/buffers/item_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sCameraData : public cObject
        {
            glm::vec3 Position;
            float _pad0 = 0;
            glm::mat4 View;
            glm::mat4 Projection;
            sViewport Viewport;
            float Exposure = 1.0f;
            float Gamma = 2.2f;
        };

        struct ENGINE_API sCameraBuffer : public sItemBuffer<sCameraData>
        {
            sCameraBuffer() : sItemBuffer<sCameraData>(K_SLOT_CAMERA) {}
        };

    }

}
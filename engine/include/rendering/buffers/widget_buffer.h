#pragma once

#include <rendering/buffers/list_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sWidgetData
        {
            glm::mat4 ModelMatrix;
            glm::vec4 Color = { 1, 1, 1, 1 };
            Boolean EnableTexture = false;
        };

        struct ENGINE_API sWidgetBuffer : public sListBuffer<sWidgetData>
        {
            sWidgetBuffer(usize count = 0) : sListBuffer<sWidgetData>(count, K_SLOT_WIDGETS) {}
        };

    }

}
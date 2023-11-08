#pragma once

#include <rendering/buffers/list_buffer.h>
#include <rendering/buffers/item_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sChar final
        {
            f32 GlyphSize = 0;
            f32 Width = 0;
            f32 Height = 0;
            f32 Left = 0;
            f32 Top = 0;
            f32 AdvanceX = 0;
            f32 AdvanceY = 0;
            f32 AtlasXOffset = 0;
            f32 AtlasYOffset = 0;
        };

        struct ENGINE_API sCharBuffer : public sListBuffer<sChar>
        {
            sCharBuffer(usize count = 0) : sListBuffer<sChar>(count, K_SLOT_CHARS) {}
        };

        struct ENGINE_API sText final
        {
            glm::mat4 ModelMatrix;
            glm::vec4 Color = { 1, 1, 1, 1 };
        };

        struct ENGINE_API sTextBuffer : public sItemBuffer<sText>
        {
            sTextBuffer() : sItemBuffer<sText>(K_SLOT_TEXT) {}
        };

    }

}
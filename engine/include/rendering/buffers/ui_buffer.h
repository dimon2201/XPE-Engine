#pragma once

#include <rendering/buffers/list_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sUiData final
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

        struct ENGINE_API sUiBuffer : public sListBuffer<sUiData>
        {
            sUiBuffer(usize count = 0) : sListBuffer<sUiData>(count, K_SLOT_UI) {}
        };

    }

}
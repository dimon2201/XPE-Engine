#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sChar final
        {
            u32 TransformIndex = 0;
            u32 CameraIndex = 0;
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

        struct ENGINE_API sTextBuffer : public sListBuffer<sChar>
        {
            sTextBuffer(usize count = 0) : sListBuffer<sChar>(count, K_SLOT_TEXT_CHARS) {}
        };

    }

}
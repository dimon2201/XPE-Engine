#pragma once

#include <rendering/buffers/instance_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API TextGlyphInstance final
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

        struct ENGINE_API TextGlyphInstanceBuffer : public StructureBuffer<TextGlyphInstance>
        {

            TextGlyphInstanceBuffer(usize count = 0) : StructureBuffer<TextGlyphInstance>(count, K_SLOT_TEXT_GLYPH_INSTANCES) {}

        };

    }

}
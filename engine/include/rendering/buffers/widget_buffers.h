#pragma once

#include <rendering/buffers/list_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sWidgetData
        {
            glm::mat4 ModelMatrix;
            glm::mat4 Projection;
            glm::vec4 Color = { 1, 1, 1, 1 };
            Boolean HasTexture = false;
            glm::vec2 AtlasUV[4] = {
                glm::vec2(0),
                glm::vec2(0),
                glm::vec2(0),
                glm::vec2(0)
            };
        };

        struct ENGINE_API sWidgetBuffer : public sListBuffer<sWidgetData>
        {
            sWidgetBuffer(usize count = 0) : sListBuffer<sWidgetData>(count, K_SLOT_WIDGETS) {}
        };

        struct ENGINE_API sChar final
        {
            u32 TextIndex = 0;
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
            glm::mat4 Projection;
            glm::vec4 Color = { 1, 1, 1, 1 };
        };

        struct ENGINE_API sTextBuffer : public sListBuffer<sText>
        {
            sTextBuffer(usize count = 0) : sListBuffer<sText>(count, K_SLOT_TEXTS) {}
        };

    }

}
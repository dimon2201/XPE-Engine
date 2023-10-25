#pragma once

#include <rendering/core/texture.h>

namespace xpe
{

    namespace render
    {

        using namespace core;

        struct ENGINE_API Font : public Object
        {
            struct Glyph final
            {
                u8 Character;
                usize Width;
                usize Height;
                ssize Left;
                ssize Top;
                usize AdvanceX;
                usize AdvanceY;
                void* BitmapData = nullptr;
                usize AtlasXOffset;
                usize AtlasYOffset;
            };

            usize GlyphCount;
            usize GlyphSize;
            f32 NewLineOffset;
            f32 WhitespaceOffset;
            usize MaxGlyphHeight;
            unordered_map<char, Font::Glyph> AlphaBet;
            Texture Atlas;

            ~Font();

            inline float GetWhitespaceWidth()
            {
                return 8.0f * ((float) AlphaBet[0x20].AdvanceX / 64.0f);
            }
        };

        Json(
            Font::Glyph,
            Character,
            Width,
            Height,
            Left,
            Top,
            AdvanceX,
            AdvanceY,
            AtlasXOffset,
            AtlasYOffset
        )

        Json(
            Font,
            GlyphCount,
            GlyphSize,
            NewLineOffset,
            WhitespaceOffset,
            MaxGlyphHeight,
            AlphaBet
        )
    }

}
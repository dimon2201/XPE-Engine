#pragma once

#include <texture_manager.hpp>

namespace xpe
{
    namespace render
    {
        struct ENGINE_API sFont : public cObject
        {
            struct sGlyph final
            {
                u8 Char;
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
            unordered_map<char, sFont::sGlyph> AlphaBet;
            cTexture Atlas;

            ~sFont();

            inline float GetWhitespaceWidth()
            {
                return 8.0f * ((float) AlphaBet[0x20].AdvanceX / 64.0f);
            }
        };

        JSON(
            sFont::sGlyph,
            Char,
            Width,
            Height,
            Left,
            Top,
            AdvanceX,
            AdvanceY,
            AtlasXOffset,
            AtlasYOffset
        )

        JSON(
            sFont,
            GlyphCount,
            GlyphSize,
            NewLineOffset,
            WhitespaceOffset,
            MaxGlyphHeight,
            AlphaBet
        )

        class ENGINE_API MFont final
        {

        public:
            static void Init();
            static void Free();
            static sFont* Load(const char* filepath, usize glyphSize);

        };
    }
}
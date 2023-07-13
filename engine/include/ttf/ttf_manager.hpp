#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

namespace xpe
{
    namespace ttf
    {
        struct Font
        {
            struct Glyph
            {
                core::u8 Character;
                core::usize Width;
                core::usize Height;
                void* BitmapData = nullptr;
            };

            core::usize GlyphCount;
        };

        class ENGINE_API TTFManager
        {
            public:
                static Font Load(const char* filePath, core::usize glyphSize);
                static void Free(const Font& font);

            private:
                static core::Boolean s_loaded;
                static FT_Library s_lib;
                static core::unordered_map<char, Font::Glyph> s_alphabet;
        };
    }
}
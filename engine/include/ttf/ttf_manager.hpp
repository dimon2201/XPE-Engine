#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <core/memory_pool.hpp>
#include <core/types.hpp>
#include <stl/map.h>

namespace xpe
{
    namespace ttf
    {
        struct xFont
        {
            struct xGlyph
            {
                core::u8 Character;
                core::usize Width;
                core::usize Height;
                void* BitmapData = nullptr;
            };

            FT_Face Face;
            core::usize GlyphCount;
            core::unordered_map<char, xFont::xGlyph> Alphabet;
        };

        class ENGINE_API TTFManager
        {
            public:
                static xFont Load(const char* filePath, core::usize glyphSize);
                static void Resize(xFont& font);
                static void Free(const xFont& font);

            private:
                static core::Boolean s_loaded;
                static FT_Library s_lib;
                static core::unordered_map<std::string, xFont> s_fonts;
        };
    }
}
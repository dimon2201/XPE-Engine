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

        class ENGINE_API TTFManager : public core::Object
        {

        public:
            static void Init();
            static void Free();

            static TTFManager& Get() {
                return *s_Instance;
            }

        public:
            Font Load(const char* filePath, core::usize glyphSize);
            void Free(const Font& font);

        private:
            static TTFManager* s_Instance;

            core::Boolean m_Loaded = core::K_FALSE;
            FT_Library m_Lib;
            core::unordered_map<char, Font::Glyph> m_AlphaBet;
        };
    }
}
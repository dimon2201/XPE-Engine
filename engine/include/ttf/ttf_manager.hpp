#pragma once

#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <rendering/core/texture.h>

namespace xpe
{
    namespace ttf
    {
        struct ENGINE_API Font
        {
            struct Glyph
            {
                core::u8 Character;
                core::usize Width;
                core::usize Height;
                core::usize Left;
                core::usize Top;
                core::usize Advance;
                void* BitmapData = nullptr;
                core::usize AtlasXOffset;
                core::usize AtlasYOffset;
            };

            core::usize GlyphCount;
            core::unordered_map<char, Font::Glyph> AlphaBet;
            render::Texture Atlas;
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
            void Free(Font& font);
            Font* GetFont(const char* filePath);

        private:
            static TTFManager* s_Instance;

            core::Boolean m_Loaded = core::K_FALSE;
            FT_Library m_Lib;
            core::unordered_map<std::string, Font> m_Fonts;
        };
    }
}
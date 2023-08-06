#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <rendering/core/texture.h>

namespace xpe
{

    namespace render
    {

        using namespace core;

        struct ENGINE_API Font final
        {
            struct Glyph
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

            FT_Face FTFace;
            usize GlyphCount;
            usize GlyphSize;
            f32 NewLineOffset;
            f32 WhitespaceOffset;
            usize MaxGlyphHeight;
            unordered_map<char, Font::Glyph> AlphaBet;
            Texture Atlas;
        };

        class ENGINE_API TTFManager final
        {

        public:
            static void Init();
            static void Free();

        public:
            static Font Load(const string& filePath, usize glyphSize);
            static Font* Resize(const string& filePath, usize glyphSize);
            static void Free(Font& font);
            static Font* GetFont(const string& filePath);
            static f32 GetWhitespaceCharWidth(Font* font);

        private:
            static bool s_Loaded;
            static FT_Library s_Lib;
            static unordered_map<string, Font>* s_Fonts;
        };
    }
}
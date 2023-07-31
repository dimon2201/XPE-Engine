#pragma once

#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <rendering/core/texture.h>

namespace xpe
{
    namespace render
    {
        class Context;
    }

    namespace ttf
    {
        struct ENGINE_API Font
        {
            struct Glyph
            {
                core::u8 Character;
                core::usize Width;
                core::usize Height;
                core::ssize Left;
                core::ssize Top;
                core::usize Advance;
                void* BitmapData = nullptr;
                core::usize AtlasXOffset;
                core::usize AtlasYOffset;
            };

            FT_Face FTFace;
            core::usize GlyphCount;
            core::usize GlyphSize;
            core::unordered_map<char, Font::Glyph> AlphaBet;
            render::Texture Atlas;
        };

        class ENGINE_API TTFManager : public core::Object
        {

        public:
            static void Init(render::Context* context);
            static void Free();

        public:
            static Font Load(const std::string& filePath, core::usize glyphSize);
            static Font* Resize(const std::string& filePath, core::usize glyphSize);
            static void Free(Font& font);
            static Font* GetFont(const std::string& filePath);

        private:
            static render::Context* s_Context;
            static core::Boolean s_Loaded;
            static FT_Library s_Lib;
            static core::unordered_map<std::string, Font>* s_Fonts;
        };
    }
}
#include <font_loader.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace xpe {

    namespace res {

        static FT_Library s_Lib;

        static unordered_map<string, FT_Face>* s_FontFaces = nullptr;

        static void FreeFontFace(FT_Face& fontFace)
        {
            FT_Done_Face(fontFace);
        }

        FontLoader::FontLoader()
        {
            if (FT_Init_FreeType(&s_Lib)) {
                LogError("Failed to initialize FreeType library!");
            }
            s_FontFaces = ::new (main_alloc(sizeof(unordered_map<string, FT_Face>))) unordered_map<string, FT_Face>();
        }

        FontLoader::~FontLoader()
        {
            for (auto& font : *s_FontFaces)
            {
                FreeFontFace(font.second);
            }
            main_free(s_FontFaces);
            FT_Done_FreeType(s_Lib);
        }

        Ref<Font> FontLoader::Load(const char* filepath, usize glyphSize)
        {
            if (m_Map.find(filepath) != m_Map.end()) {
                Ref<Font> fontRef;
                fontRef.Create(*m_Map[filepath]);
                return fontRef;
            }

            Ref<Font> fontRef;
            fontRef.Create(Font());
            Font& font = *fontRef;
            FT_Face fontFace = {};

            if (FT_New_Face(s_Lib, filepath, 0, &fontFace) == 0)
            {
                if (FT_Set_Pixel_Sizes(fontFace, 0, glyphSize) == 0)
                {
                    font.GlyphCount = 0;
                    font.GlyphSize = glyphSize;

                    usize textureWidth = 0;
                    usize textureHeight = 0;
                    usize xOffset = 0;
                    usize maxGlyphHeight = 0;
                    int lastIndex = 0;

                    for (int c = 0; c < 256; c++)
                    {
                        FT_UInt gi = FT_Get_Char_Index(fontFace, (char)c);

                        if (FT_Load_Glyph(fontFace, gi, FT_LOAD_DEFAULT) == 0)
                        {
                            font.GlyphCount += 1;

                            FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_NORMAL);

                            Font::Glyph glyph;
                            glyph.Character = (char)c;
                            glyph.Width = fontFace->glyph->bitmap.width;
                            glyph.Height = fontFace->glyph->bitmap.rows;
                            glyph.Left = fontFace->glyph->bitmap_left;
                            glyph.Top = fontFace->glyph->bitmap_top;
                            glyph.AdvanceX = fontFace->glyph->advance.x;
                            glyph.AdvanceY = 0.0f;
                            glyph.BitmapData = main_alloc(glyph.Width * glyph.Height);

                            int x = 0;
                            int y = 0;
                            for (int i = 0; i < (int)glyph.Height; i++)
                            {
                                for (int j = 0; j < (int)glyph.Width; j++)
                                {
                                    unsigned char p = fontFace->glyph->bitmap.buffer[i * fontFace->glyph->bitmap.pitch + j];
                                    ((unsigned char*)glyph.BitmapData)[i * fontFace->glyph->bitmap.pitch + j] = p;
                                }
                            }

                            font.AlphaBet.insert({ (char)c, glyph });

                            xOffset += glyph.Width + 1;
                            if (glyph.Height > maxGlyphHeight) {
                                maxGlyphHeight = glyph.Height;
                            }

                            if (xOffset > 1024)
                            {
                                if (textureWidth < xOffset)
                                {
                                    textureWidth = xOffset;
                                }

                                textureHeight += maxGlyphHeight + 1;
                                xOffset = 0;
                                maxGlyphHeight = 0;
                            }
                        }
                    }

                    font.Atlas.Type = render::Texture::eType::TEXTURE_2D;
                    font.Atlas.Width = textureWidth;
                    font.Atlas.Height = textureHeight;
                    font.Atlas.Depth = 1;
                    font.Atlas.Channels = 1;
                    font.Atlas.Format = eTextureFormat::R8;
                    font.Atlas.Slot = 0;
                    font.Atlas.MostDetailedMip = 0;
                    font.Atlas.Layers.push_back({});
                    font.Atlas.Layers[0].RowByteSize = font.Atlas.Width;
                    font.Atlas.Layers[0].Pixels = main_alloc(font.Atlas.Width * font.Atlas.Height);
                    memset(font.Atlas.Layers[0].Pixels, 0, font.Atlas.Width * font.Atlas.Height);
                    const TextureLayer& layer = font.Atlas.Layers[0];

                    // Fill atlas texture with glyphs
                    xOffset = 0;
                    usize yOffset = 0;
                    u8* pixelsU8 = (u8*)layer.Pixels;
                    for (auto& glyph : font.AlphaBet)
                    {
                        glyph.second.AtlasXOffset = xOffset;
                        glyph.second.AtlasYOffset = yOffset;

                        for (int y = 0; y < glyph.second.Height; y++)
                        {
                            for (int x = 0; x < glyph.second.Width; x++)
                            {
                                int glyphPixelIndex = x + (y * glyph.second.Width);
                                int pixelIndex = (xOffset + x) + ((yOffset + y) * textureWidth);
                                pixelsU8[pixelIndex] = ((core::u8*)glyph.second.BitmapData)[glyphPixelIndex];
                            }
                        }

                        xOffset += glyph.second.Width + 1;
                        if (glyph.second.Height > maxGlyphHeight) {
                            maxGlyphHeight = glyph.second.Height;
                        }

                        if (xOffset > 1024)
                        {
                            yOffset += maxGlyphHeight + 1;
                            xOffset = 0;
                            maxGlyphHeight = 0;
                        }
                    }

                    // Find max glyph height
                    font.MaxGlyphHeight = 0;
                    for (auto& glyph : font.AlphaBet) {
                        if (glyph.second.Height > font.MaxGlyphHeight) {
                            font.MaxGlyphHeight = glyph.second.Height;
                        }
                    }

                    font.Atlas.Init();
                    s_FontFaces->insert({ filepath, fontFace });
                    m_Map.insert({ filepath, fontRef });
                }
            }
            else {
                LogError("Error creating FreeType font face!");
            }

            return fontRef;
        }

        Ref<Font> FontLoader::Resize(const char *filepath, usize glyphSize)
        {
            if (m_Map.find(filepath) == m_Map.end()) {
                LogError("Font doesn't exist in cache.");
                return { nullptr };
            }

            auto& fontRef = m_Map[filepath];
            auto& font = *fontRef;

            FT_Face& fontFace = s_FontFaces->at(filepath);

            if (FT_Set_Pixel_Sizes(fontFace, 0, glyphSize) == 0)
            {
                font.GlyphCount = 0;
                font.GlyphSize = glyphSize;

                core::usize textureWidth = 0;
                core::usize textureHeight = 0;
                core::usize xOffset = 0;
                core::usize maxGlyphHeight = 0;
                int lastIndex = 0;

                for (int c = 0; c < 256; c++)
                {
                    FT_UInt gi = FT_Get_Char_Index(fontFace, (char)c);

                    if (FT_Load_Glyph(fontFace, gi, FT_LOAD_DEFAULT) == 0)
                    {
                        font.GlyphCount += 1;

                        FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_NORMAL);

                        Font::Glyph glyph;
                        glyph.Character = (char)c;
                        glyph.Width = fontFace->glyph->bitmap.width;
                        glyph.Height = fontFace->glyph->bitmap.rows;
                        glyph.Left = fontFace->glyph->bitmap_left;
                        glyph.Top = fontFace->glyph->bitmap_top;
                        glyph.AdvanceX = fontFace->glyph->advance.x;
                        glyph.AdvanceY = 0.0f;
                        glyph.BitmapData = main_alloc(glyph.Width * glyph.Height);
                        memset(glyph.BitmapData, 0, glyph.Width * glyph.Height);

                        int x = 0;
                        int y = 0;
                        for (int i = 0; i < (int)glyph.Height; i++)
                        {
                            for (int j = 0; j < (int)glyph.Width; j++)
                            {
                                unsigned char p = fontFace->glyph->bitmap.buffer[i * fontFace->glyph->bitmap.pitch + j];
                                ((unsigned char*)glyph.BitmapData)[i * fontFace->glyph->bitmap.pitch + j] = p;
                            }
                        }

                        font.AlphaBet.insert({ (char)c, glyph });

                        xOffset += glyph.Width + 1;
                        if (glyph.Height > maxGlyphHeight) {
                            maxGlyphHeight = glyph.Height;
                        }

                        if (xOffset > 1024)
                        {
                            if (textureWidth < xOffset)
                            {
                                textureWidth = xOffset;
                            }

                            textureHeight += maxGlyphHeight + 1;
                            xOffset = 0;
                            maxGlyphHeight = 0;
                        }
                    }
                }

                font.Atlas.Type = render::Texture::eType::TEXTURE_2D;
                font.Atlas.Width = textureWidth;
                font.Atlas.Height = textureHeight;
                font.Atlas.Depth = 1;
                font.Atlas.Channels = 1;
                font.Atlas.Format = eTextureFormat::R8;
                font.Atlas.Slot = 0;
                font.Atlas.MostDetailedMip = 0;
                font.Atlas.Layers.push_back({});
                font.Atlas.Layers[0].RowByteSize = font.Atlas.Width;
                font.Atlas.Layers[0].Pixels = main_alloc(font.Atlas.Width * font.Atlas.Height);
                memset(font.Atlas.Layers[0].Pixels, 0, font.Atlas.Width * font.Atlas.Height);
                const render::TextureLayer& layer = font.Atlas.Layers[0];

                // Fill atlas texture with glyphs
                xOffset = 0;
                maxGlyphHeight = 0;
                core::usize yOffset = 0;
                core::u8* pixelsU8 = (core::u8*)layer.Pixels;
                for (auto& glyph : font.AlphaBet)
                {
                    glyph.second.AtlasXOffset = xOffset;
                    glyph.second.AtlasYOffset = yOffset;

                    for (int y = 0; y < glyph.second.Height; y++)
                    {
                        for (int x = 0; x < glyph.second.Width; x++)
                        {
                            int glyphPixelIndex = x + (y * glyph.second.Width);
                            int pixelIndex = (xOffset + x) + ((yOffset + y) * textureWidth);
                            pixelsU8[pixelIndex] = ((core::u8*)glyph.second.BitmapData)[glyphPixelIndex];
                        }
                    }

                    xOffset += glyph.second.Width + 1;
                    if (glyph.second.Height > maxGlyphHeight) {
                        maxGlyphHeight = glyph.second.Height;
                    }

                    if (xOffset > 1024)
                    {
                        yOffset += maxGlyphHeight + 1;
                        xOffset = 0;
                        maxGlyphHeight = 0;
                    }
                }

                // Find max glyph height
                font.MaxGlyphHeight = 0;
                for (auto& glyph : font.AlphaBet) {
                    if (glyph.second.Height > font.MaxGlyphHeight) {
                        font.MaxGlyphHeight = glyph.second.Height;
                    }
                }

                font.Atlas.Init();
                s_FontFaces->insert({ filepath, fontFace });
            }

            return fontRef;
        }

    }

}
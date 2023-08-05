#include <rendering/text/ttf_manager.hpp>

namespace xpe {

    namespace render {

        bool TTFManager::s_Loaded = false;
        FT_Library TTFManager::s_Lib;
        unordered_map<string, Font>* TTFManager::s_Fonts = nullptr;

        void TTFManager::Init() {
            s_Fonts = new unordered_map<string, Font>();
        }

        void TTFManager::Free() {
            FT_Done_FreeType(s_Lib);
            delete s_Fonts;
        }

        Font TTFManager::Load(const string& filePath, usize glyphSize)
        {
            Font font = {};

            if (!s_Loaded)
            {
                if (FT_Init_FreeType(&s_Lib))
                {
                    LogError("Error initializing FreeType!");
                }
                else
                {
                    s_Loaded = true;
                }
            }

            if (FT_New_Face(s_Lib, filePath.c_str(), 0, &font.FTFace) == 0)
            {
                if (FT_Set_Pixel_Sizes(font.FTFace, 0, glyphSize) == 0)
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
                        FT_UInt gi = FT_Get_Char_Index(font.FTFace, (char)c);

                        if (FT_Load_Glyph(font.FTFace, gi, FT_LOAD_DEFAULT) == 0)
                        {
                            font.GlyphCount += 1;

                            FT_Render_Glyph(font.FTFace->glyph, FT_RENDER_MODE_NORMAL);

                            Font::Glyph glyph;
                            glyph.Character = (char)c;
                            glyph.Width = font.FTFace->glyph->bitmap.width;
                            glyph.Height = font.FTFace->glyph->bitmap.rows;
                            glyph.Left = font.FTFace->glyph->bitmap_left;
                            glyph.Top = font.FTFace->glyph->bitmap_top;
                            glyph.AdvanceX = font.FTFace->glyph->advance.x;
                            glyph.AdvanceY = 0.0f;
                            glyph.BitmapData = alloc(glyph.Width * glyph.Height);

                            int x = 0;
                            int y = 0;
                            for (int i = 0; i < (int)glyph.Height; i++)
                            {
                                for (int j = 0; j < (int)glyph.Width; j++)
                                {
                                    unsigned char p = font.FTFace->glyph->bitmap.buffer[i * font.FTFace->glyph->bitmap.pitch + j];
                                    ((unsigned char*)glyph.BitmapData)[i * font.FTFace->glyph->bitmap.pitch + j] = p;
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
                    font.Atlas.Format = render::Texture::eFormat::R8;
                    font.Atlas.Slot = 0;
                    font.Atlas.MostDetailedMip = 0;
                    font.Atlas.Layers.push_back({});
                    font.Atlas.Layers[0].RowByteSize = font.Atlas.Width;
                    font.Atlas.Layers[0].Pixels = alloc(font.Atlas.Width * font.Atlas.Height);
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

                    TextureManager::InitTexture(font.Atlas);

                    s_Fonts->insert({ filePath, font });
                }
            }
            else
            {
                LogError("Error creating FreeType font face!");
            }

            return font;
        }

        Font* TTFManager::Resize(const string& filePath, usize glyphSize)
        {
            Font* font = GetFont(filePath);
            if (font == nullptr) {
                return nullptr;
            }

            if (font->Atlas.Layers[0].Pixels != nullptr) {
                dealloc(font->Atlas.Layers[0].Pixels);
            }

            for (auto& glyph : font->AlphaBet) {
                dealloc(glyph.second.BitmapData);
                glyph.second.BitmapData = nullptr;
            }

            font->AlphaBet.clear();

            TextureManager::FreeTexture(font->Atlas);

            if (FT_Set_Pixel_Sizes(font->FTFace, 0, glyphSize) == 0)
            {
                font->GlyphCount = 0;
                font->GlyphSize = glyphSize;

                core::usize textureWidth = 0;
                core::usize textureHeight = 0;
                core::usize xOffset = 0;
                core::usize maxGlyphHeight = 0;
                int lastIndex = 0;

                for (int c = 0; c < 256; c++)
                {
                    FT_UInt gi = FT_Get_Char_Index(font->FTFace, (char)c);

                    if (FT_Load_Glyph(font->FTFace, gi, FT_LOAD_DEFAULT) == 0)
                    {
                        font->GlyphCount += 1;

                        FT_Render_Glyph(font->FTFace->glyph, FT_RENDER_MODE_NORMAL);

                        Font::Glyph glyph;
                        glyph.Character = (char)c;
                        glyph.Width = font->FTFace->glyph->bitmap.width;
                        glyph.Height = font->FTFace->glyph->bitmap.rows;
                        glyph.Left = font->FTFace->glyph->bitmap_left;
                        glyph.Top = font->FTFace->glyph->bitmap_top;
                        glyph.AdvanceX = font->FTFace->glyph->advance.x;
                        glyph.AdvanceY = 0.0f;
                        glyph.BitmapData = alloc(glyph.Width * glyph.Height);
                        memset(glyph.BitmapData, 0, glyph.Width * glyph.Height);

                        int x = 0;
                        int y = 0;
                        for (int i = 0; i < (int)glyph.Height; i++)
                        {
                            for (int j = 0; j < (int)glyph.Width; j++)
                            {
                                unsigned char p = font->FTFace->glyph->bitmap.buffer[i * font->FTFace->glyph->bitmap.pitch + j];
                                ((unsigned char*)glyph.BitmapData)[i * font->FTFace->glyph->bitmap.pitch + j] = p;
                            }
                        }

                        font->AlphaBet.insert({ (char)c, glyph });

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

                font->Atlas.Type = render::Texture::eType::TEXTURE_2D;
                font->Atlas.Width = textureWidth;
                font->Atlas.Height = textureHeight;
                font->Atlas.Depth = 1;
                font->Atlas.Channels = 1;
                font->Atlas.Format = render::Texture::eFormat::R8;
                font->Atlas.Slot = 0;
                font->Atlas.MostDetailedMip = 0;
                font->Atlas.Layers.push_back({});
                font->Atlas.Layers[0].RowByteSize = font->Atlas.Width;
                font->Atlas.Layers[0].Pixels = alloc(font->Atlas.Width * font->Atlas.Height);
                memset(font->Atlas.Layers[0].Pixels, 0, font->Atlas.Width * font->Atlas.Height);
                const render::TextureLayer& layer = font->Atlas.Layers[0];

                // Fill atlas texture with glyphs
                xOffset = 0;
                maxGlyphHeight = 0;
                core::usize yOffset = 0;
                core::u8* pixelsU8 = (core::u8*)layer.Pixels;
                for (auto& glyph : font->AlphaBet)
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

                render::TextureManager::InitTexture(font->Atlas);
            }

            return font;
        }

        void TTFManager::Free(Font& font)
        {
            TextureManager::FreeTexture(font.Atlas);

            for (auto& glyph : font.AlphaBet) {
                dealloc(glyph.second.BitmapData);
                glyph.second.BitmapData = nullptr;
            }

            font.AlphaBet.clear();

            FT_Done_Face(font.FTFace);
        }

        Font* TTFManager::GetFont(const string& filePath)
        {
            auto it = s_Fonts->find(filePath);

            if (it != s_Fonts->end())
            {
                return &it->second;
            }

            return nullptr;
        }

    }

}
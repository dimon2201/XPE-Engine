#include <ttf/ttf_manager.hpp>
#include <core/main_allocator.h>

xpe::ttf::TTFManager* xpe::ttf::TTFManager::s_Instance = nullptr;

void xpe::ttf::TTFManager::Init() {
    s_Instance = new TTFManager();
}

void xpe::ttf::TTFManager::Free() {
    delete s_Instance;
}

xpe::ttf::Font xpe::ttf::TTFManager::Load(const char* filePath, core::usize glyphSize)
{
    Font font = {};

    if (m_Loaded == core::K_FALSE)
    {
        if (FT_Init_FreeType(&m_Lib))
        {
            LogError("Error initializing FreeType!");
        }
        else
        {
            m_Loaded = core::K_TRUE;
        }
    }

    if (FT_New_Face(m_Lib, filePath, 0, &font.FTFace) == 0)
    {
        if (FT_Set_Pixel_Sizes(font.FTFace, 0, glyphSize) == 0)
        {
            font.GlyphCount = 0;
            font.GlyphSize = glyphSize;

            core::usize textureWidth = 0;
            core::usize textureHeight = 0;
            core::usize xOffset = 0;
            core::usize maxGlyphHeight = 0;

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
                    glyph.Advance = font.FTFace->glyph->advance.x;
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
            const render::TextureLayer& layer = font.Atlas.Layers[0];

            // Fill atlas texture with glyphs
            xOffset = 0;
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
                }
            }

            render::TextureManager::InitTexture(font.Atlas);

            m_Fonts.insert({ std::string(filePath), font });
        }
    }
    else
    {
        LogError("Error creating FreeType font face!");
    }

    return font;
}

xpe::ttf::Font* xpe::ttf::TTFManager::Resize(const char* filePath, core::usize glyphSize)
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

    if (FT_Set_Pixel_Sizes(font->FTFace, 0, glyphSize) == 0)
    {
        font->GlyphCount = 0;
        font->GlyphSize = glyphSize;

        core::usize textureWidth = 0;
        core::usize textureHeight = 0;
        core::usize xOffset = 0;
        core::usize maxGlyphHeight = 0;

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
                glyph.Advance = font->FTFace->glyph->advance.x;
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
            }
        }
    }

    return font;
}

void xpe::ttf::TTFManager::Free(Font& font)
{
    render::TextureManager::FreeTexture(font.Atlas);

    for (auto& glyph : font.AlphaBet) {
        dealloc(glyph.second.BitmapData);
        glyph.second.BitmapData = nullptr;
    }

    font.AlphaBet.clear();

    FT_Done_Face(font.FTFace);
}

xpe::ttf::Font* xpe::ttf::TTFManager::GetFont(const char* filePath)
{
    auto it = m_Fonts.find(std::string(filePath));

    if (it != m_Fonts.end())
    {
        return &it->second;
    }

    return nullptr;
}
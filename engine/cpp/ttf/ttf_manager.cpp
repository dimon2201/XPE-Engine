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

    FT_Face face;
    if (FT_New_Face(m_Lib, filePath, 0, &face) == 0)
    {
        if (FT_Set_Pixel_Sizes(face, 0, glyphSize) == 0)
        {
            core::usize textureWidth = 0;
            core::usize textureHeight = 0;
            core::usize xOffset = 0;

            for (int c = 0; c < 256; c++)
            {
                FT_UInt gi = FT_Get_Char_Index(face, (char)c);

                if (FT_Load_Glyph(face, gi, FT_LOAD_DEFAULT) == 0)
                {
                    font.GlyphCount += 1;

                    FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

                    Font::Glyph glyph;
                    glyph.Character = (char)c;
                    glyph.Width = face->glyph->bitmap.width;
                    glyph.Height = face->glyph->bitmap.rows;
                    glyph.Left = face->glyph->bitmap_left;
                    glyph.Top = face->glyph->bitmap_top;
                    glyph.Advance = face->glyph->advance.x;
                    glyph.BitmapData = alloc(face->glyph->bitmap.pitch * glyph.Height);

                    int x = 0;
                    int y = 0;
                    for (int i = 0; i < (int)glyph.Height; i++)
                    {
                        for (int j = 0; j < (int)glyph.Width; j++)
                        {
                            unsigned char p = face->glyph->bitmap.buffer[i * face->glyph->bitmap.pitch + j];
                            ((unsigned char*)glyph.BitmapData)[i * face->glyph->bitmap.pitch + j] = p;
                        }
                    }

                    font.AlphaBet.insert({ (char)c, glyph });

                    xOffset += glyph.Width + 1;

                    if (xOffset > 1024)
                    {
                        if (textureWidth < xOffset)
                        {
                            textureWidth = xOffset;
                        }

                        textureHeight += glyph.Height + 1;
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
            render::TextureLayer layer;
            layer.Pixels = alloc(font.Atlas.Width * font.Atlas.Height);
            font.Atlas.Layers.push_back(layer);
            font.Atlas.MostDetailedMip = 0;

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

                if (xOffset > 1024)
                {
                    yOffset += glyph.second.Height + 1;
                    xOffset = 0;
                }
            }

            render::TextureManager::InitTexture(font.Atlas);

            m_Fonts.insert({ std::string(filePath), font });
        }

        FT_Done_Face(face);
    }
    else
    {
        LogError("Error creating FreeType font face!");
    }

    return font;
}

void xpe::ttf::TTFManager::Free(Font& font)
{
    // render::TextureManager::FreeTexture(font.Atlas);

    for (auto& glyph : font.AlphaBet)
    {
        dealloc(glyph.second.BitmapData);
    }

    font.AlphaBet.clear();
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
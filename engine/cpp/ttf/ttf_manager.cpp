#include <ttf/ttf_manager.hpp>

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

                    xOffset += glyph.Width;

                    if (xOffset > 1024)
                    {
                        textureWidth = xOffset;
                        textureHeight += glyph.Height;
                    }
                }
            }

            font.Atlas.Type = render::Texture::eType::TEXTURE_2D;
            font.Atlas.Width = textureWidth;
            font.Atlas.Height = textureHeight;
            font.Atlas.Depth = 1;
            font.Atlas.ChannelCount = 4;
            font.Atlas.Format = render::Texture::eFormat::RGBA8;
            font.Atlas.OnMemoryPool = core::K_TRUE;
            render::TextureLayer textureLayer;
            textureLayer.Pixels = alloc(font.Atlas.Width * font.Atlas.Height * 4);
            font.Atlas.Layers.push_back(textureLayer);
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
    render::TextureManager::FreeTexture(font.Atlas);

    for (auto& glyph : font.AlphaBet)
    {
        dealloc(glyph.second.BitmapData);
    }

    font.AlphaBet.clear();
}
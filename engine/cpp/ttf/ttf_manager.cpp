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
                }
            }
        }

        FT_Done_Face(face);
    }
    else
    {
        LogError("Error creating FreeType font face!");
    }

    return font;
}

void xpe::ttf::TTFManager::Free(const xpe::ttf::Font& font)
{
    for (auto& glyph : m_AlphaBet)
    {
        if (glyph.second.BitmapData != nullptr)
        {
            dealloc(glyph.second.BitmapData);
        }
    }
}
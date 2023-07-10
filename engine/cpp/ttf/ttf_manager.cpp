#include <ttf/ttf_manager.hpp>

xpe::core::Boolean xpe::ttf::TTFManager::s_loaded = core::K_FALSE;
FT_Library xpe::ttf::TTFManager::s_lib;
xpe::core::unordered_map<char, xpe::ttf::xFont::xGlyph> xpe::ttf::TTFManager::s_alphabet;

xpe::ttf::xFont xpe::ttf::TTFManager::Load(const char* filePath, core::usize glyphSize)
{
    xFont font = {};

    if (s_loaded == core::K_FALSE)
    {
        if (FT_Init_FreeType(&s_lib))
        {
            LogError("Error initializing FreeType!");
        }
        else
        {
            s_loaded = core::K_TRUE;
        }
    }

    FT_Face face;
    if (FT_New_Face(s_lib, filePath, 0, &face) == 0)
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

                    xFont::xGlyph glyph;
                    glyph.Character = (char)c;
                    glyph.Width = face->glyph->bitmap.width;
                    glyph.Height = face->glyph->bitmap.rows;
                    glyph.BitmapData = core::MemoryPoolManager::Allocate(face->glyph->bitmap.pitch * glyph.Height);

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

void xpe::ttf::TTFManager::Free(const xpe::ttf::xFont& font)
{
    for (auto& glyph : s_alphabet)
    {
        if (glyph.second.BitmapData != nullptr)
        {
            core::MemoryPoolManager::Free(glyph.second.BitmapData);
        }
    }
}
#include <core/logger.h>
#include <ttf/ttf_manager.hpp>

xpe::core::Boolean xpe::ttf::TTFManager::s_loaded = core::K_FALSE;
FT_Library xpe::ttf::TTFManager::s_lib;
xpe::core::unordered_map<std::string, xpe::ttf::xFont> xpe::ttf::TTFManager::s_fonts;

xpe::ttf::xFont xpe::ttf::TTFManager::Load(const char* filePath, core::usize glyphSize)
{
    xFont font = {};

    if (s_loaded == core::K_FALSE)
    {
        if (FT_Init_FreeType(&s_lib))
        {
            LogInfo("Error initializing FreeType!");
        }
        else
        {
            s_loaded = core::K_TRUE;
        }
    }

    if (FT_New_Face(s_lib, filePath, 0, &font.Face) == 0)
    {
        if (FT_Set_Pixel_Sizes(font.Face, 0, glyphSize) == 0)
        {
            for (int c = 0; c < 256; c++)
            {
                FT_UInt gi = FT_Get_Char_Index(font.Face, (char)c);

                if (FT_Load_Glyph(font.Face, gi, FT_LOAD_DEFAULT) == 0)
                {
                    font.GlyphCount += 1;

                    FT_Render_Glyph(font.Face->glyph, FT_RENDER_MODE_NORMAL);

                    xFont::xGlyph glyph;
                    glyph.Character = (char)c;
                    glyph.Width = font.Face->glyph->bitmap.width;
                    glyph.Height = font.Face->glyph->bitmap.rows;
                    glyph.BitmapData = core::MemoryPoolManager::Allocate(font.Face->glyph->bitmap.pitch * glyph.Height);

                    int x = 0;
                    int y = 0;
                    for (int i = 0; i < (int)glyph.Height; i++)
                    {
                        for (int j = 0; j < (int)glyph.Width; j++)
                        {
                            unsigned char p = font.Face->glyph->bitmap.buffer[i * font.Face->glyph->bitmap.pitch + j];
                            ((unsigned char*)glyph.BitmapData)[i * font.Face->glyph->bitmap.pitch + j] = p;
                        }
                    }

                    font.Alphabet.insert({ (char)c, glyph });
                }
            }
        }

        s_fonts.insert({ std::string(filePath), font });
    }
    else
    {
        LogInfo("Error creating FreeType font face!");
    }

    return font;
}

void xpe::ttf::TTFManager::Free(const xpe::ttf::xFont& font)
{
    for (auto& glyph : font.Alphabet)
    {
        if (glyph.second.BitmapData != nullptr)
        {
            core::MemoryPoolManager::Free(glyph.second.BitmapData);
        }
    }

    FT_Done_Face(font.Face);
}
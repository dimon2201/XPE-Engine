#include <rendering/font/font.hpp>

namespace xpe {

    namespace render {

        sFont::~sFont()
        {
            for (auto& glyph : AlphaBet) {
                main_free(glyph.second.BitmapData);
                glyph.second.BitmapData = nullptr;
            }
            AlphaBet.clear();
        }

    }

}
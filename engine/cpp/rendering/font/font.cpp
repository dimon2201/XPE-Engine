#include <rendering/font/font.hpp>

namespace xpe {

    namespace render {

        Font::~Font()
        {
            for (auto& glyph : AlphaBet) {
                dealloc(glyph.second.BitmapData);
                glyph.second.BitmapData = nullptr;
            }
            AlphaBet.clear();
        }

    }

}
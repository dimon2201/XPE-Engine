#pragma once

#include <build.h>

#include <rendering/font/font.hpp>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace render;
        using namespace math;

        class LOADER_API cFontLoader final
        {
        public:
            static void Init();
            static void Free();
            static sFont* Load(const char* filepath, usize glyphSize);
        };

    }

}
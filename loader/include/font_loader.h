#pragma once

#include <build.h>

#include <rendering/font/font.hpp>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace render;
        using namespace math;

        class LOADER_API FontLoader final
        {
        public:
            static void Init();
            static void Free();
            static Font* Load(const char* filepath, usize glyphSize);
        };

    }

}
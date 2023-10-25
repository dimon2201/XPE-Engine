#pragma once

#include <build.h>

#include <rendering/font/font.hpp>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace render;
        using namespace math;

        class LOADER_API FontLoader : public Object
        {

        public:
            FontLoader();
            ~FontLoader();

            Ref<Font> Load(const char* filepath, usize glyphSize);
            Ref<Font> Resize(const char* filepath, usize glyphSize);

        private:
            unordered_map<string, Ref<Font>> m_Map;

        };

    }

}
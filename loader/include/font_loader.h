#pragma once

#include <build.h>

#include <rendering/storages/font_storage.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace render;
        using namespace math;

        class LOADER_API FontLoader : public Object
        {

        public:
            FontLoader(FontStorage* storage);
            ~FontLoader();

            Ref<Font> Load(const char* filepath, usize glyphSize);
            Ref<Font> Resize(const char* filepath, usize glyphSize);

        private:
            FontStorage* m_Storage = nullptr;
        };

    }

}
#pragma once

#include <rendering/font/font.hpp>

namespace xpe {

    namespace render {

        struct ENGINE_API FontStorage : public Object
        {

            Ref<Font> Add(const string& name, const Font& font);

            void Remove(const string& name);

            inline Ref<Font>& Get(const string& name) { return m_Fonts[name]; }

            void Flush(Font& font);

            void Clear();

            inline f32 GetWhitespaceCharWidth(Font& font)
            {
                return 8.0f * ((f32)font.AlphaBet[0x20].AdvanceX / 64.0f);
            }

            inline Ref<Font>& operator [](const string& name)
            {
                return Get(name);
            }

        private:
            unordered_map<string, Ref<Font>> m_Fonts;
        };

    }

}
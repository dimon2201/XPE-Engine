#include <rendering/storages/font_storage.h>

namespace xpe {

    namespace render {

        Ref<Font> FontStorage::Add(const string &name, const Font &font)
        {
            Ref<Font> fontRef;
            fontRef.Create(font);
            m_Fonts.insert({ name, fontRef });
            return fontRef;
        }

        void FontStorage::Remove(const string &name)
        {
            m_Fonts.erase(name);
        }

        void FontStorage::Flush(Font &font)
        {
            font.Atlas.Flush();
        }

        void FontStorage::Clear()
        {
            m_Fonts.clear();
        }

    }

}
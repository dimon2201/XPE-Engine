#include <rendering/storages/texture_storage.h>

namespace xpe {

    namespace render {

        Ref<Texture> TextureStorage::Add(const string &name, const Texture& texture)
        {
            Ref<Texture> textureRef;
            textureRef.Create(texture);
            m_Textures.insert({ name, textureRef });
            return textureRef;
        }

        void TextureStorage::Remove(const string &name)
        {
            auto it = m_Textures.find(name);
            if (it != m_Textures.end()) {
                m_Textures.erase(it);
            }
        }

        void TextureStorage::Clear()
        {
            m_Textures.clear();
        }

    }

}
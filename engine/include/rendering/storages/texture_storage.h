#pragma once

#include <rendering/core/texture.h>

namespace xpe {

    namespace render {

        class ENGINE_API TextureStorage : public Object
        {

        public:
            Ref<Texture> Add(const string& name, const Texture& texture);

            void Remove(const string& name);

            inline Ref<Texture>& Get(const string& name) { return m_Textures[name]; }

            void Clear();

            inline Ref<Texture>& operator[](const string& name) { return Get(name); }

        protected:
            unordered_map<string, Ref<Texture>> m_Textures;
        };

    }

}
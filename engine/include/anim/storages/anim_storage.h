#pragma once

#include <anim/anim.h>

namespace xpe {

    namespace anim {

        class ENGINE_API AnimStorage : public Object
        {

        public:
            Ref<Animation> Add(const string& name, const Animation& texture);

            void Remove(const string& name);

            inline Ref<Animation>& Get(const string& name) { return m_Anims[name]; }

            void Clear();

            inline Ref<Animation>& operator[](const string& name) { return Get(name); }

        protected:
            unordered_map<string, Ref<Animation>> m_Anims;
        };

    }

}
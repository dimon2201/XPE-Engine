#pragma once

#include <anim/skelet.h>

namespace xpe {

    namespace anim {

        using namespace render;

        struct ENGINE_API SkeletStorage : public Object
        {
            Ref<Skelet> Add(const string& name, const Skelet& skelet);

            void Remove(const string& name);

            inline Ref<Skelet>& Get(const string& name) { return m_Skelets[name]; }

            void Clear();

        private:
            unordered_map<string, Ref<Skelet>> m_Skelets;
        };

    }

}
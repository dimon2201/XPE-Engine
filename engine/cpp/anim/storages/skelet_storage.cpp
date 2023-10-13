#include <anim/storages/skelet_storage.h>

namespace xpe {

    namespace anim {

        Ref<Skelet> SkeletStorage::Add(const string &name, const Skelet& skelet)
        {
            Ref<Skelet> skeletRef;
            skeletRef.Create(skelet);
            skeletRef->BoneBuffer.Resize(skelet.Bones.size());
            m_Map.insert({name, skeletRef });
            return skeletRef;
        }

        void SkeletStorage::Remove(const string &name)
        {
            auto it = m_Map.find(name);
            if (it != m_Map.end()) {
                m_Map.erase(it);
            }
        }

        void SkeletStorage::Clear()
        {
            m_Map.clear();
        }

    }

}
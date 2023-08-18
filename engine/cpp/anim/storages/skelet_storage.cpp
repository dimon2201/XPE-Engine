#include <anim/storages/skelet_storage.h>

namespace xpe {

    namespace anim {

        Ref<Skelet> SkeletStorage::Add(const string &name, const Skelet& skelet)
        {
            Ref<Skelet> skeletRef;
            skeletRef.Create(skelet);
            skeletRef->BoneBuffer.Resize(skelet.Bones.size());
            m_Skelets.insert({ name, skeletRef });
            return skeletRef;
        }

        void SkeletStorage::Remove(const string &name)
        {
            auto it = m_Skelets.find(name);
            if (it != m_Skelets.end()) {
                m_Skelets.erase(it);
            }
        }

        void SkeletStorage::Clear()
        {
            m_Skelets.clear();
        }

    }

}
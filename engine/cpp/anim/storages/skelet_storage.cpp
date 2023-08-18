#include <anim/storages/skelet_storage.h>

namespace xpe {

    namespace anim {

        Ref<Skelet> SkeletStorage::Add(const string &name, const Skelet& skelet)
        {
            Ref<Skelet> skeletRef;
            skeletRef.Create(skelet);
            m_Skelets.insert({ name, skeletRef });

            Ref<BoneBuffer> boneBuffer;
            boneBuffer.Create(skelet.Bones.size());
            m_BoneBuffers.insert({ skeletRef.Get(), boneBuffer });

            return skeletRef;
        }

        void SkeletStorage::Remove(const string &name)
        {
            auto it = m_Skelets.find(name);
            if (it != m_Skelets.end()) {
                m_BoneBuffers.erase(it->second.Get());
                m_Skelets.erase(it);
            }
        }

        void SkeletStorage::Clear()
        {
            m_Skelets.clear();
            m_BoneBuffers.clear();
        }

    }

}
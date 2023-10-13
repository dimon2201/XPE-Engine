#include <anim/storages/anim_storage.h>

namespace xpe {

    namespace anim {

        Ref<Animation> AnimStorage::Add(const string &name, const Animation& animation)
        {
            Ref<Animation> animRef;
            animRef.Create(animation);
            m_Map.insert({name, animRef });
            return animRef;
        }

        void AnimStorage::Remove(const string &name)
        {
            auto it = m_Map.find(name);
            if (it != m_Map.end()) {
                m_Map.erase(it);
            }
        }

        void AnimStorage::Clear()
        {
            m_Map.clear();
        }

    }

}
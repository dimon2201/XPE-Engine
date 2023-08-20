#include <anim/storages/anim_storage.h>

namespace xpe {

    namespace anim {

        Ref<Animation> AnimStorage::Add(const string &name, const Animation& animation)
        {
            Ref<Animation> animRef;
            animRef.Create(animation);
            m_Anims.insert({ name, animRef });
            return animRef;
        }

        void AnimStorage::Remove(const string &name)
        {
            auto it = m_Anims.find(name);
            if (it != m_Anims.end()) {
                m_Anims.erase(it);
            }
        }

        void AnimStorage::Clear()
        {
            m_Anims.clear();
        }

    }

}
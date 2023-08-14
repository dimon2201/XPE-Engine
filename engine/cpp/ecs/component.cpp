#include <ecs/component.h>

namespace xpe {

    namespace ecs {

        Component* ComponentStorage::GetAddress(usize componentSize, const string& tag)
        {
            u64 id = Hash(tag);
            usize size = m_Components.size();
            usize step = componentSize;

            for (usize i = 0 ; i < size ; i += step)
            {
                auto* component = reinterpret_cast<Component*>(&m_Components[i]);
                u64 cid = Hash(component->GetTag());
                if (cid == id) {
                    return component;
                }
            }

            return nullptr;
        }

        void ComponentStorage::Clear()
        {
            m_Components.clear();
        }

    }

}
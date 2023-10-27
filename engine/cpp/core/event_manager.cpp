#include <core/event_manager.h>

namespace xpe {

    namespace core {

        EventManager* EventManager::s_Instance = nullptr;

        void EventManager::Init() {
            s_Instance = new EventManager();
        }

        void EventManager::Free()
        {
            delete s_Instance;
        }

        EventManager& EventManager::Get()
        {
            return *s_Instance;
        }

    }

}
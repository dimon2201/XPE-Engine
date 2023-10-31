#include <core/event_manager.h>

namespace xpe {

    namespace core {

        cEventManager* cEventManager::s_Instance = nullptr;

        void cEventManager::Init() {
            s_Instance = new cEventManager();
        }

        void cEventManager::Free()
        {
            delete s_Instance;
        }

        cEventManager& cEventManager::Get()
        {
            return *s_Instance;
        }

    }

}
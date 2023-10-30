#include <rendering/skybox_manager.h>

namespace xpe {

    namespace render {

        SkyboxManager* SkyboxManager::s_Instance = nullptr;

        void SkyboxManager::Init()
        {
            s_Instance = new SkyboxManager();
        }

        void SkyboxManager::Free()
        {
            delete s_Instance;
        }

        SkyboxManager &SkyboxManager::Get()
        {
            if (s_Instance == nullptr) {
                Init();
            }
            return *s_Instance;
        }

    }

}
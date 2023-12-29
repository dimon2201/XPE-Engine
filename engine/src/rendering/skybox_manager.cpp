#include <rendering/skybox_manager.hpp>

namespace xpe {

    namespace render {

        cSkyboxManager* cSkyboxManager::s_Instance = nullptr;

        void cSkyboxManager::Init()
        {
            s_Instance = new cSkyboxManager();
        }

        void cSkyboxManager::Free()
        {
            delete s_Instance;
        }

        cSkyboxManager &cSkyboxManager::Get()
        {
            if (s_Instance == nullptr) {
                Init();
            }
            return *s_Instance;
        }

    }

}
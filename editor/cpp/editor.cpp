#include <editor.h>
#include <launcher.h>

namespace focus {

    namespace core {

        void cEditor::Init()
        {
            LogInfo("cEditor::Init()");
            cProjectManager::Init();
            cProjectManager::CreateProject("Chess");
            cProjectManager::Callback = this;
            cProjectManager::StartWatching("Chess");
            // todo later I will move it into Input keys and UI
//        cProjectManager::OpenInCLion("Chess");
//        cProjectManager::OpenInСMake("Chess");
//        cProjectManager::OpenInVS("Chess");
//        cProjectManager::OpenInVSCode("Chess");
//        cProjectManager::LaunchGame("Chess", Project::eBuildType::DEBUG_VS);
        }

        void cEditor::Update()
        {
            cApp* gameReload = GameReloaded.load();
            if (gameReload != nullptr) {
                GameReloaded.store(nullptr);
            }
        }

        void cEditor::Free()
        {
            LogInfo("cEditor::Free()");
            cProjectManager::Free();
        }

    }

}

xpe::core::cApp* CreateApplication()
{
    cApp* app = new focus::core::cEditor();

    // read app configs
    // app->Config = string("EditorConfig"); todo: compilator error
    if (!xpe::res::ReadJsonFile("config/editor_config.json", app->Config))
    {
        FMT_ASSERT(false, "Failed to read app config from config/config.json file. Please provide config file!");
        return 0;
    }

    return app;
}
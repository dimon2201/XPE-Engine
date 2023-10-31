#include <editor.h>
#include <launcher.h>

namespace focus {

    namespace core {

        void Editor::Init()
        {
            LogInfo("Editor::Init()");
            ProjectManager::Init();
            ProjectManager::CreateProject("Chess");
            ProjectManager::Callback = this;
            ProjectManager::StartWatching("Chess");
            // todo later I will move it into Input keys and UI
//        ProjectManager::OpenInCLion("Chess");
//        ProjectManager::OpenInСMake("Chess");
//        ProjectManager::OpenInVS("Chess");
//        ProjectManager::OpenInVSCode("Chess");
//        ProjectManager::LaunchGame("Chess", Project::eBuildType::DEBUG_VS);
        }

        void Editor::Update()
        {
            Application* gameReload = GameReloaded.load();
            if (gameReload != nullptr) {
                GameReloaded.store(nullptr);
            }
        }

        void Editor::Free()
        {
            LogInfo("Editor::Free()");
            ProjectManager::Free();
        }

    }

}

xpe::core::Application* CreateApplication()
{
    Application* app = new focus::core::Editor();

    // read app configs
    // app->Config = string("EditorConfig"); todo: compilator error
    if (!xpe::res::ReadJsonFile("config/editor_config.json", app->Config))
    {
        FMT_ASSERT(false, "Failed to read app config from config/config.json file. Please provide config file!");
        return 0;
    }

    return app;
}
#include <editor.h>
#include <launcher.h>

namespace focus {

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
        Game* gameReload = GameReloaded.load();
        if (gameReload != nullptr) {
            GameReloaded.store(nullptr);
            LoadGame(gameReload);
        }
    }

    void Editor::Free()
    {
        LogInfo("Editor::Free()");
        ProjectManager::Free();
    }

    void Editor::LoadGame(Game* game)
    {
        LogInfo("Editor::LoadGame()");
        m_Game->Free();
        m_Game = game;
        InitGame();
    }

}
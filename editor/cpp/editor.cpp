#include <editor.h>
#include <launcher.h>

namespace focus {

    void Editor::Init()
    {
        LogInfo("Editor::Init()");
        ProjectManager::Init();
        ProjectManager::CreateProject("Test");
        ProjectManager::Callback = this;
    }

    void Editor::Update()
    {
        ProjectManager::UpdateWatches();
    }

    void Editor::Free()
    {
        LogInfo("Editor::Free()");
        ProjectManager::Free();
    }

    void Editor::GameCodeReloaded(Game* game)
    {
        LogInfo("Editor::GameCodeReloaded()");

        if (game == nullptr) {
            LogError("Failed to reload game code. Game code is null");
            return;
        }

        m_Game->Free();
        m_Game = game;
        InitGame();
    }

}
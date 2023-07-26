#include <project_manager.h>

#include <core/game.h>

#include <rendering/core/shader.h>

#ifdef WINDOWS

#include <Windows.h>

#endif

namespace focus {

    using namespace xpe::os;
    using namespace xpe::render;

    ProjectStorage* ProjectManager::s_Storage = nullptr;
    ProjectManagerCallback* ProjectManager::Callback = nullptr;

    void ProjectManager::Init()
    {
        s_Storage = new ProjectStorage();
    }

    void ProjectManager::Free()
    {
        delete s_Storage;
    }

    Project* ProjectManager::CreateProject(const string& title)
    {
        // create path to project
        hstringstream ss;
        ss << "workspace" << "/" << title;
        hstring projectPath = ss.str();

        if (s_Storage->Projects.find(title) != s_Storage->Projects.end() || FileManager::Exists(projectPath.c_str()))
        {
            LogError("Failed to create project {}. Project already exists.", title);
            SetupProject(title);
            return &s_Storage->Projects[title];
        }

        // create project folder
        FileManager::CreateDir(projectPath.c_str());

        // copy all from template to new project folder
        FileManager::CopyDirs("templates/Game", projectPath.c_str());

        // copy engine into new project folder
        ss = {};
        ss << projectPath << "/" << "engine";
        hstring enginePath = ss.str();
        FileManager::CopyDirs("engine", enginePath.c_str());

        // create path to cmake Launcher
        ss = {};
        ss << projectPath << "/" << "Launcher" << "/" << "CMakeLists.txt";
        hstring cmakeLauncherPath = ss.str();

        // create path to cmake Code
        ss = {};
        ss << projectPath << "/" << "Code" << "/" << "CMakeLists.txt";
        hstring cmakeCodePath = ss.str();

        // create path to cmake Project
        ss = {};
        ss << projectPath << "/" << "CMakeLists.txt";
        hstring cmakeProjectPath = ss.str();

        // update cmake for Launcher
        string cmakeLauncher = FileManager::ReadFile(cmakeLauncherPath.c_str());
        cmakeLauncher = std::regex_replace(cmakeLauncher, std::regex("\\Game"), title);
        FileManager::WriteFile(cmakeLauncherPath.c_str(), cmakeLauncher);

        // update cmake for Code
        string cmakeCode = FileManager::ReadFile(cmakeCodePath.c_str());
        cmakeCode = std::regex_replace(cmakeCode, std::regex("\\Game"), title);
        FileManager::WriteFile(cmakeCodePath.c_str(), cmakeCode);

        // update cmake for Project
        string cmakeProject = FileManager::ReadFile(cmakeProjectPath.c_str());
        cmakeProject = std::regex_replace(cmakeProject, std::regex("\\Game"), title);
        FileManager::WriteFile(cmakeProjectPath.c_str(), cmakeProject);

        SetupProject(title);

        return &s_Storage->Projects[title];
    }

    void ProjectManager::SetupProject(const string &title)
    {
        Project project;
        project.Title = title;

        s_Storage->Projects[title] = project;

        hstring projectPath = GetProjectPath(title);

        // create build dirs for Visual Studio
        hstringstream ss;
        ss << projectPath << "/build";
        FileManager::CreateDir(ss.str().c_str());
        ss = {};
        ss << projectPath << "/build/bin";
        FileManager::CreateDir(ss.str().c_str());
        ss = {};
        ss << projectPath << "/build/bin/Debug";
        FileManager::CreateDir(ss.str().c_str());
        ss << "/" << title << "Code.dll";
        FileManager::WriteFile(ss.str().c_str(), "");
        ss = {};
        ss << projectPath << "/build/bin/Release";
        FileManager::CreateDir(ss.str().c_str());
        ss << "/" << title << "Code.dll";
        FileManager::WriteFile(ss.str().c_str(), "");

        // create build dirs for CLion
        ss = {};
        ss << projectPath << "/build-debug";
        FileManager::CreateDir(ss.str().c_str());
        ss = {};
        ss << projectPath << "/build-debug/bin";
        FileManager::CreateDir(ss.str().c_str());
        ss << "/" << title << "Code.dll";
        FileManager::WriteFile(ss.str().c_str(), "");
        ss = {};
        ss << projectPath << "/build-release";
        FileManager::CreateDir(ss.str().c_str());
        ss = {};
        ss << projectPath << "/build-release/bin";
        FileManager::CreateDir(ss.str().c_str());
        ss << "/" << title << "Code.dll";
        FileManager::WriteFile(ss.str().c_str(), "");

        DirectoryWatcher& directoryWatcher = *FileManager::CreateDirectoryWatch(title);
        MultiFileWatcher& multiFileWatcher = *FileManager::CreateMultiFileWatch(title);

        // watch for Visual Studio build
        WatchProjectGameCodeDLL(multiFileWatcher, project.Title, "build/bin/Debug");
        WatchProjectGameCodeDLL(multiFileWatcher, project.Title, "build/bin/Release");

        // watch for CLion build
        WatchProjectGameCodeDLL(multiFileWatcher, project.Title, "build-debug/bin");
        WatchProjectGameCodeDLL(multiFileWatcher, project.Title, "build-release/bin");

        // watch for game project source folders
        WatchProjectShaders(directoryWatcher, project.Title);
        WatchProjectResources(directoryWatcher, project.Title);
    }

    static void GameCodeDLLModified(
            void* thiz,
            const string& watchpath,
            const string& filepath
    ) {
        LogInfo("ProjectManager: Game project {} has been changed!", filepath);
        ProjectManager::LoadGameCode(filepath);
    }

    static void ShaderFileModified(
            void* thiz,
            const string& watchpath,
            const string& filepath
    ) {
        LogInfo("ProjectManager: Shader file {} has been changed!", filepath);
        ShaderManager::ReloadStage(filepath.c_str());
    }

    static void ModelFileModified(
            void* thiz,
            const string& watchpath,
            const string& filepath
    ) {
        LogInfo("ProjectManager: Model file {} has been changed!", filepath);
    }

    static void FontFileModified(
            void* thiz,
            const string& watchpath,
            const string& filepath
    ) {
        LogInfo("ProjectManager: Font file {} has been changed!", filepath);
    }

    static void MaterialFileModified(
            void* thiz,
            const string& watchpath,
            const string& filepath
    ) {
        LogInfo("ProjectManager: Material file {} has been changed!", filepath);
    }

    static void SkyboxFileModified(
            void* thiz,
            const string& watchpath,
            const string& filepath
    ) {
        LogInfo("ProjectManager: Skybox file {} has been changed!", filepath);
    }

    void ProjectManager::UpdateWatches()
    {
        FileManager::UpdateDirectoryWatchers();
        FileManager::UpdateMultiFileWatchers();
    }

    void ProjectManager::WatchProjectGameCodeDLL(MultiFileWatcher& watcher, const string &title, const char* buildPath)
    {
        hstringstream ss;
        ss << "workspace/" << title << "/" << buildPath << "/" << title << "Code.dll";
        hstring gameCodeDllPath = ss.str();

        auto& watch = watcher.AddWatch(gameCodeDllPath.c_str());
        watch.FileModifiedEvent = { GameCodeDLLModified, 1 };
    }

    void ProjectManager::WatchProjectShaders(DirectoryWatcher& watcher, const string &title)
    {
        hstringstream ss;
        ss << "workspace/" << title << "/" << "shaders";
        hstring shadersPath = ss.str();

        auto& watch = watcher.AddWatch(shadersPath.c_str());
        watch.FileModifiedEvent = { ShaderFileModified, 1 };
        watch.FileAddedEvent = { ShaderFileModified, 1 };
        watch.FileDeletedEvent = { ShaderFileModified, 1 };
    }

    void ProjectManager::WatchProjectResources(DirectoryWatcher& watcher, const string &title)
    {
        hstringstream ss;
        ss << "workspace/" << title << "/resources/" << "models";
        hstring modelsPath = ss.str();

        {
            auto& watch = watcher.AddWatch(modelsPath.c_str());
            watch.FileModifiedEvent = { ModelFileModified, 1 };
            watch.FileAddedEvent = { ModelFileModified, 1 };
            watch.FileDeletedEvent = { ModelFileModified, 1 };
        }

        ss = {};
        ss << "workspace/" << title << "/resources/" << "fonts";
        hstring fontsPath = ss.str();

        {
            auto& watch = watcher.AddWatch(fontsPath.c_str());
            watch.FileModifiedEvent = { FontFileModified, 1 };
            watch.FileAddedEvent = { FontFileModified, 1 };
            watch.FileDeletedEvent = { FontFileModified, 1 };
        }

        ss = {};
        ss << "workspace/" << title << "/resources/" << "materials";
        hstring materialsPath = ss.str();

        {
            auto& watch = watcher.AddWatch(materialsPath.c_str());
            watch.FileModifiedEvent = { MaterialFileModified, 1 };
            watch.FileAddedEvent = { MaterialFileModified, 1 };
            watch.FileDeletedEvent = { MaterialFileModified, 1 };
        }

        ss = {};
        ss << "workspace/" << title << "/resources/" << "skybox";
        hstring skyboxPath = ss.str();

        {
            auto& watch = watcher.AddWatch(skyboxPath.c_str());
            watch.FileModifiedEvent = { SkyboxFileModified, 1 };
            watch.FileAddedEvent = { SkyboxFileModified, 1 };
            watch.FileDeletedEvent = { SkyboxFileModified, 1 };
        }
    }

    hstring ProjectManager::GetProjectPath(const string &title)
    {
        hstringstream ss;
        ss << "workspace/" << title;
        return ss.str();
    }

    void ProjectManager::LoadGameCode(const string &dllpath)
    {
        Game* game = nullptr;

        // load DLL
        HINSTANCE library = static_cast<HINSTANCE>(LoadLibrary(dllpath.c_str()));
        if (library == nullptr) {
            LogError("Failed to load game code DLL library from {}", dllpath);
            return;
        }

        // load Game object
        game = ((CreateGameFn) GetProcAddress(static_cast<HINSTANCE>(library), "CreateGame"))();

        // free DLL
        FreeLibrary(static_cast<HINSTANCE>(library));

        // check Game object
        if (game == nullptr) {
            LogError("Failed to load Game object from game code DLL process function {} from {}", "CreateGame", dllpath);
            return;
        }

        // dispatch Game object
        if (Callback != nullptr) {
            Callback->GameCodeReloaded(game);
        }
    }

}
#include <project_manager.h>
#include <os/process.h>

#include <core/game.h>
#include <rendering/core/shader.h>

namespace focus {

    using namespace xpe::os;
    using namespace xpe::render;

    ProjectStorage* ProjectManager::s_Storage = nullptr;
    ProjectManagerCallback* ProjectManager::Callback = nullptr;

    static const std::unordered_map<Project::eBuildType, const char*> s_BinPathTable = {
            { Project::eBuildType::DEBUG_VS, "build/bin/Debug" },
            { Project::eBuildType::RELEASE_VS, "build/bin/Release" },
            { Project::eBuildType::DEBUG_CLION, "build-debug/bin" },
            { Project::eBuildType::RELEASE_CLION, "build-release/bin" },
    };

    void ProjectManager::Init()
    {
        s_Storage = new ProjectStorage();
    }

    void ProjectManager::Free()
    {
        for (auto& project : s_Storage->Projects) {
            StopWatching(project.first);
            delete project.second.FilesWatcher;
            delete project.second.DirsWatcher;
        }
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

        // create path to main file of Launcher project
        ss = {};
        ss << projectPath << "/Launcher/" << "main.cpp";
        hstring cppLauncherPath = ss.str();

        // create path to main file of Code project
        ss = {};
        ss << projectPath << "/Code/cpp/game.cpp";
        hstring cppCodePath = ss.str();
        ss = {};
        ss << projectPath << "/Code/include/game.h";
        hstring headerCodePath = ss.str();

        // update Launcher CMakeLists file
        string cmakeLauncher = FileManager::ReadFile(cmakeLauncherPath.c_str());
        cmakeLauncher = std::regex_replace(cmakeLauncher, std::regex("\\Game"), title);
        FileManager::WriteFile(cmakeLauncherPath.c_str(), cmakeLauncher);

        // update Launcher cpp file
        string cppLauncher = FileManager::ReadFile(cppLauncherPath.c_str());
        cppLauncher = std::regex_replace(cppLauncher, std::regex("\\TemplateGame"), title);
        cppLauncher = std::regex_replace(cppLauncher, std::regex("\\Template"), title);
        FileManager::WriteFile(cppLauncherPath.c_str(), cppLauncher);

        // update Code CMakeLists file
        string cmakeCode = FileManager::ReadFile(cmakeCodePath.c_str());
        cmakeCode = std::regex_replace(cmakeCode, std::regex("\\Game"), title);
        FileManager::WriteFile(cmakeCodePath.c_str(), cmakeCode);

        // update Code cpp file
        string cppCode = FileManager::ReadFile(cppCodePath.c_str());
        cppCode = std::regex_replace(cppCode, std::regex("\\TemplateGame"), title);
        FileManager::WriteFile(cppCodePath.c_str(), cppCode);

        // update Code header file
        string headerCode = FileManager::ReadFile(headerCodePath.c_str());
        headerCode = std::regex_replace(headerCode, std::regex("\\TemplateGame"), title);
        FileManager::WriteFile(headerCodePath.c_str(), headerCode);

        // update Project CMakeLists file
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
        project.FilesWatcher = new MultiFileWatcher(title);
        project.DirsWatcher = new DirectoryWatcher(title);

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
        ss << "/" << title << "Code_Edit.dll";
        FileManager::WriteFile(ss.str().c_str(), "");
        ss = {};
        ss << projectPath << "/build/bin/Release";
        FileManager::CreateDir(ss.str().c_str());
        ss << "/" << title << "Code_Edit.dll";
        FileManager::WriteFile(ss.str().c_str(), "");

        // create build dirs for CLion
        ss = {};
        ss << projectPath << "/build-debug";
        FileManager::CreateDir(ss.str().c_str());
        ss = {};
        ss << projectPath << "/build-debug/bin";
        FileManager::CreateDir(ss.str().c_str());
        ss << "/" << title << "Code_Edit.dll";
        FileManager::WriteFile(ss.str().c_str(), "");
        ss = {};
        ss << projectPath << "/build-release";
        FileManager::CreateDir(ss.str().c_str());
        ss = {};
        ss << projectPath << "/build-release/bin";
        FileManager::CreateDir(ss.str().c_str());
        ss << "/" << title << "Code_Edit.dll";
        FileManager::WriteFile(ss.str().c_str(), "");

        // watch for Visual Studio build
        WatchProjectGameCodeDLL(*project.FilesWatcher, project.Title, "build/bin/Debug");
        WatchProjectGameCodeDLL(*project.FilesWatcher, project.Title, "build/bin/Release");

        // watch for CLion build
        WatchProjectGameCodeDLL(*project.FilesWatcher, project.Title, "build-debug/bin");
        WatchProjectGameCodeDLL(*project.FilesWatcher, project.Title, "build-release/bin");

        // watch for game project source folders
        WatchProjectShaders(*project.DirsWatcher, project.Title);
        WatchProjectResources(*project.DirsWatcher, project.Title);
    }

    static void GameCodeDLLModified(
            void* thiz,
            const string& watchpath,
            const string& filepath
    ) {
        LogInfo("ProjectManager: Game code of {} has been changed!", filepath);
//        hstring dllName = FileManager::GetFullFileName(filepath.c_str());
//        FileManager::CopyFile(filepath.c_str(), dllName.c_str());
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

    void ProjectManager::WatchProjectGameCodeDLL(MultiFileWatcher& watcher, const string &title, const char* buildPath)
    {
        hstringstream ss;
        ss << "workspace/" << title << "/" << buildPath << "/" << title << "Code_Edit.dll";
        hstring gameCodeDllPath = ss.str();

        auto& watch = watcher.AddWatch(gameCodeDllPath.c_str());
        watch.FileModifiedEventBuffer.AddEvent(nullptr, GameCodeDLLModified, 1);
    }

    void ProjectManager::WatchProjectShaders(DirectoryWatcher& watcher, const string &title)
    {
        hstringstream ss;
        ss << "workspace/" << title << "/" << "shaders";
        hstring shadersPath = ss.str();

        auto& watch = watcher.AddWatch(shadersPath.c_str());
        watch.FileModifiedEventBuffer.AddEvent(nullptr, ShaderFileModified, 1);
        watch.FileAddedEventBuffer.AddEvent(nullptr, ShaderFileModified, 1);
        watch.FileDeletedEventBuffer.AddEvent(nullptr, ShaderFileModified, 1);
    }

    void ProjectManager::WatchProjectResources(DirectoryWatcher& watcher, const string &title)
    {
        hstringstream ss;
        ss << "workspace/" << title << "/resources/" << "models";
        hstring modelsPath = ss.str();

        {
            auto& watch = watcher.AddWatch(modelsPath.c_str());
            watch.FileModifiedEventBuffer.AddEvent(nullptr, ModelFileModified, 1);
            watch.FileAddedEventBuffer.AddEvent(nullptr, ModelFileModified, 1);
            watch.FileDeletedEventBuffer.AddEvent(nullptr, ModelFileModified, 1);
        }

        ss = {};
        ss << "workspace/" << title << "/resources/" << "fonts";
        hstring fontsPath = ss.str();

        {
            auto& watch = watcher.AddWatch(fontsPath.c_str());
            watch.FileModifiedEventBuffer.AddEvent(nullptr, FontFileModified, 1);
            watch.FileAddedEventBuffer.AddEvent(nullptr, FontFileModified, 1);
            watch.FileDeletedEventBuffer.AddEvent(nullptr, FontFileModified, 1);
        }

        ss = {};
        ss << "workspace/" << title << "/resources/" << "materials";
        hstring materialsPath = ss.str();

        {
            auto& watch = watcher.AddWatch(materialsPath.c_str());
            watch.FileModifiedEventBuffer.AddEvent(nullptr, MaterialFileModified, 1);
            watch.FileAddedEventBuffer.AddEvent(nullptr, MaterialFileModified, 1);
            watch.FileDeletedEventBuffer.AddEvent(nullptr, MaterialFileModified, 1);
        }

        ss = {};
        ss << "workspace/" << title << "/resources/" << "skybox";
        hstring skyboxPath = ss.str();

        {
            auto& watch = watcher.AddWatch(skyboxPath.c_str());
            watch.FileModifiedEventBuffer.AddEvent(nullptr, SkyboxFileModified, 1);
            watch.FileAddedEventBuffer.AddEvent(nullptr, SkyboxFileModified, 1);
            watch.FileDeletedEventBuffer.AddEvent(nullptr, SkyboxFileModified, 1);
        }
    }

    hstring ProjectManager::GetProjectPath(const string &title)
    {
        hstringstream ss;
        ss << "workspace/" << title;
        return ss.str();
    }

    hstring ProjectManager::GetVSBuildPath(const string &title)
    {
        hstringstream ss;
        ss << "workspace/" << title << "/build";
        return ss.str();
    }

    hstring ProjectManager::GetLaunchPath(const string &title, Project::eBuildType buildType)
    {
        hstringstream ss;
        ss << "workspace/" << title << "/" << s_BinPathTable.at(buildType);
        return ss.str();
    }

    void ProjectManager::LoadGameCode(const string &dllpath)
    {
        Game* game = Process::LoadObjectFromDLL<Game>(dllpath.c_str(), "CreateGame");
        if (game == nullptr) {
            LogError("Failed to load game code from DLL {}", dllpath);
            return;
        }
        // dispatch Game object with atomic thread-safe approach
        if (Callback != nullptr) {
            Callback->GameReloaded.store(game);
        }
    }

    void ProjectManager::OpenInСMake(const string &title)
    {
        std::thread thread([title]() {
            hstringstream ss;

            hstring projectPath = GetProjectPath(title);
            hstring buildPath = GetVSBuildPath(title);
            if (!FileManager::Exists(projectPath.c_str())) {
                LogError("Failed to open in CMake GUI for a game project {}. Path {} not found", title, projectPath);
                return;
            }

            ss << "cmake-gui";
            ss << " -B " << FileManager::GetAbsolutePath(buildPath.c_str());
            ss << " -S " << FileManager::GetAbsolutePath(projectPath.c_str());
            system(ss.str().c_str());
        });
        thread.detach();
    }

    void ProjectManager::OpenInCLion(const string &title)
    {
        std::thread thread([title]() {
            hstringstream ss;

            hstring projectPath = GetProjectPath(title);
            if (!FileManager::Exists(projectPath.c_str())) {
                LogError("Failed to open in CLion IDE a game project {}. Path {} not found", title, projectPath);
                return;
            }

            ss << "clion64.exe " << FileManager::GetAbsolutePath(projectPath.c_str());
            system(ss.str().c_str());
        });
        thread.detach();
    }

    void ProjectManager::OpenInVS(const string &title)
    {
        std::thread thread([title]() {
            hstringstream ss;

            hstring projectPath = GetProjectPath(title);
            if (!FileManager::Exists(projectPath.c_str())) {
                LogError("Failed to open in Visual Studio IDE a game project {}. Path {} not found", title, projectPath);
                return;
            }

            ss << "start devenv " << FileManager::GetAbsolutePath(projectPath.c_str());
            system(ss.str().c_str());
        });
        thread.detach();
    }

    void ProjectManager::OpenInVSCode(const string &title)
    {
        std::thread thread([title]() {
            hstringstream ss;

            hstring projectPath = GetProjectPath(title);
            if (!FileManager::Exists(projectPath.c_str())) {
                LogError("Failed to open in VS Code a game project {}. Path {} not found", title, projectPath);
                return;
            }

            ss << "code " << FileManager::GetAbsolutePath(projectPath.c_str());
            system(ss.str().c_str());
        });
        thread.detach();
    }

    void ProjectManager::LaunchGame(const string &title, Project::eBuildType buildType)
    {
        std::thread thread([title, buildType]() {
            hstringstream ss;

            hstring launchPath = GetLaunchPath(title, buildType);
            ss << launchPath << "/" << title << ".exe";
            hstring exePath = ss.str();

            if (!FileManager::Exists(exePath.c_str())) {
                LogError("Failed to launch a game executable of {}. Path {} not found", title, exePath);
                return;
            }

            Process::StartExe(FileManager::GetAbsolutePath(exePath.c_str()).c_str());
        });
        thread.detach();
    }

    void ProjectManager::StartWatching(const string& title)
    {
        auto project = s_Storage->Projects.find(title);

        if (project == s_Storage->Projects.end()) {
            LogError("Failed to start watching for game project {}. Project is absent!", title);
            return;
        }

        project->second.FilesWatcher->Start();
        project->second.DirsWatcher->Start();
    }

    void ProjectManager::StopWatching(const string& title)
    {
        auto project = s_Storage->Projects.find(title);

        if (project == s_Storage->Projects.end()) {
            LogError("Failed to stop watching for game project {}. Project is absent!", title);
            return;
        }

        project->second.FilesWatcher->Stop();
        project->second.DirsWatcher->Stop();
    }

}
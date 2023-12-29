#include <core/project_manager.hpp>
#include <core/process_manager.hpp>

#include <rendering/core/shader.hpp>

namespace focus {

    namespace core {

        using namespace xpe::render;

        sProjectStorage* cProjectManager::s_Storage = nullptr;
        sProjectManagerCallback* cProjectManager::Callback = nullptr;

        static const std::unordered_map<sProject::eBuildType, const char*> s_BinPathTable = {
                {sProject::eBuildType::DEBUG_VS,      "build/bin/Debug" },
                {sProject::eBuildType::RELEASE_VS,    "build/bin/Release" },
                {sProject::eBuildType::DEBUG_CLION,   "build-debug/bin" },
                {sProject::eBuildType::RELEASE_CLION, "build-release/bin" },
        };

        void cProjectManager::Init()
        {
            s_Storage = new sProjectStorage();
        }

        void cProjectManager::Free()
        {
            for (auto& project : s_Storage->Projects) {
                StopWatching(project.first);
                delete project.second.FilesWatcher;
                delete project.second.DirsWatcher;
            }
            delete s_Storage;
        }

        sProject* cProjectManager::CreateProject(const string& title)
        {
            // create path to project
            hstringstream ss;
            ss << "workspace" << "/" << title;
            hstring projectPath = ss.str();

            if (s_Storage->Projects.find(title) != s_Storage->Projects.end() || cFileManager::Exists(projectPath.c_str()))
            {
                LogError("Failed to create project {}. Project already exists.", title);
                SetupProject(title);
                return &s_Storage->Projects[title];
            }

            // create project folder
            cFileManager::CreateDir(projectPath.c_str());

            // copy all from template to new project folder
            cFileManager::CopyDirs("templates/Game", projectPath.c_str());

            // copy engine into new project folder
            ss = {};
            ss << projectPath << "/" << "engine";
            hstring enginePath = ss.str();
            cFileManager::CopyDirs("engine", enginePath.c_str());

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
            string cmakeLauncher = cFileManager::ReadFile(cmakeLauncherPath.c_str());
            cmakeLauncher = std::regex_replace(cmakeLauncher, std::regex("\\Game"), title);
            cFileManager::WriteFile(cmakeLauncherPath.c_str(), cmakeLauncher);

            // update Launcher cpp file
            string cppLauncher = cFileManager::ReadFile(cppLauncherPath.c_str());
            cppLauncher = std::regex_replace(cppLauncher, std::regex("\\TemplateGame"), title);
            cppLauncher = std::regex_replace(cppLauncher, std::regex("\\Template"), title);
            cFileManager::WriteFile(cppLauncherPath.c_str(), cppLauncher);

            // update Code CMakeLists file
            string cmakeCode = cFileManager::ReadFile(cmakeCodePath.c_str());
            cmakeCode = std::regex_replace(cmakeCode, std::regex("\\Game"), title);
            cFileManager::WriteFile(cmakeCodePath.c_str(), cmakeCode);

            // update Code cpp file
            string cppCode = cFileManager::ReadFile(cppCodePath.c_str());
            cppCode = std::regex_replace(cppCode, std::regex("\\TemplateGame"), title);
            cFileManager::WriteFile(cppCodePath.c_str(), cppCode);

            // update Code header file
            string headerCode = cFileManager::ReadFile(headerCodePath.c_str());
            headerCode = std::regex_replace(headerCode, std::regex("\\TemplateGame"), title);
            cFileManager::WriteFile(headerCodePath.c_str(), headerCode);

            // update Project CMakeLists file
            string cmakeProject = cFileManager::ReadFile(cmakeProjectPath.c_str());
            cmakeProject = std::regex_replace(cmakeProject, std::regex("\\Game"), title);
            cFileManager::WriteFile(cmakeProjectPath.c_str(), cmakeProject);

            SetupProject(title);

            return &s_Storage->Projects[title];
        }

        void cProjectManager::SetupProject(const string &title)
        {
            sProject project;
            project.Title = title;
            project.FilesWatcher = new cMultiFileWatcher(title);
            project.DirsWatcher = new cDirectoryWatcher(title);

            s_Storage->Projects[title] = project;

            hstring projectPath = GetProjectPath(title);

            // create build dirs for Visual Studio
            hstringstream ss;
            ss << projectPath << "/build";
            cFileManager::CreateDir(ss.str().c_str());
            ss = {};
            ss << projectPath << "/build/bin";
            cFileManager::CreateDir(ss.str().c_str());
            ss = {};
            ss << projectPath << "/build/bin/Debug";
            cFileManager::CreateDir(ss.str().c_str());
            ss << "/" << title << "Code_Edit.dll";
            cFileManager::WriteFile(ss.str().c_str(), "");
            ss = {};
            ss << projectPath << "/build/bin/Release";
            cFileManager::CreateDir(ss.str().c_str());
            ss << "/" << title << "Code_Edit.dll";
            cFileManager::WriteFile(ss.str().c_str(), "");

            // create build dirs for CLion
            ss = {};
            ss << projectPath << "/build-debug";
            cFileManager::CreateDir(ss.str().c_str());
            ss = {};
            ss << projectPath << "/build-debug/bin";
            cFileManager::CreateDir(ss.str().c_str());
            ss << "/" << title << "Code_Edit.dll";
            cFileManager::WriteFile(ss.str().c_str(), "");
            ss = {};
            ss << projectPath << "/build-release";
            cFileManager::CreateDir(ss.str().c_str());
            ss = {};
            ss << projectPath << "/build-release/bin";
            cFileManager::CreateDir(ss.str().c_str());
            ss << "/" << title << "Code_Edit.dll";
            cFileManager::WriteFile(ss.str().c_str(), "");

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
            LogInfo("cProjectManager: Game code of {} has been changed!", filepath);
            cProjectManager::LoadGameCode(filepath);
        }

        static void ShaderFileModified(
                void* thiz,
                const string& watchpath,
                const string& filepath
        ) {
            LogInfo("cProjectManager: sShader file {} has been changed!", filepath);
            cShaderManager::ReloadStage(filepath.c_str());
        }

        static void ModelFileModified(
                void* thiz,
                const string& watchpath,
                const string& filepath
        ) {
            LogInfo("cProjectManager: sModel file {} has been changed!", filepath);
        }

        static void FontFileModified(
                void* thiz,
                const string& watchpath,
                const string& filepath
        ) {
            LogInfo("cProjectManager: sFont file {} has been changed!", filepath);
        }

        static void MaterialFileModified(
                void* thiz,
                const string& watchpath,
                const string& filepath
        ) {
            LogInfo("cProjectManager: Material file {} has been changed!", filepath);
        }

        static void SkyboxFileModified(
                void* thiz,
                const string& watchpath,
                const string& filepath
        ) {
            LogInfo("cProjectManager: sSkybox file {} has been changed!", filepath);
        }

        void cProjectManager::WatchProjectGameCodeDLL(cMultiFileWatcher& watcher, const string &title, const char* buildPath)
        {
            hstringstream ss;
            ss << "workspace/" << title << "/" << buildPath << "/" << title << "Code_Edit.dll";
            hstring gameCodeDllPath = ss.str();

            auto& watch = watcher.AddWatch(gameCodeDllPath.c_str());
            watch.FileModifiedEvents.AddEvent(nullptr, GameCodeDLLModified, 1);
        }

        void cProjectManager::WatchProjectShaders(cDirectoryWatcher& watcher, const string &title)
        {
            hstringstream ss;
            ss << "workspace/" << title << "/" << "shaders";
            hstring shadersPath = ss.str();

            auto& watch = watcher.AddWatch(shadersPath.c_str());
            watch.FileModifiedEvents.AddEvent(nullptr, ShaderFileModified, 1);
            watch.FileAddedEvents.AddEvent(nullptr, ShaderFileModified, 1);
            watch.FileDeletedEvents.AddEvent(nullptr, ShaderFileModified, 1);
        }

        void cProjectManager::WatchProjectResources(cDirectoryWatcher& watcher, const string &title)
        {
            hstringstream ss;
            ss << "workspace/" << title << "/res/" << "models";
            hstring modelsPath = ss.str();

            {
                auto& watch = watcher.AddWatch(modelsPath.c_str());
                watch.FileModifiedEvents.AddEvent(nullptr, ModelFileModified, 1);
                watch.FileAddedEvents.AddEvent(nullptr, ModelFileModified, 1);
                watch.FileDeletedEvents.AddEvent(nullptr, ModelFileModified, 1);
            }

            ss = {};
            ss << "workspace/" << title << "/res/" << "fonts";
            hstring fontsPath = ss.str();

            {
                auto& watch = watcher.AddWatch(fontsPath.c_str());
                watch.FileModifiedEvents.AddEvent(nullptr, FontFileModified, 1);
                watch.FileAddedEvents.AddEvent(nullptr, FontFileModified, 1);
                watch.FileDeletedEvents.AddEvent(nullptr, FontFileModified, 1);
            }

            ss = {};
            ss << "workspace/" << title << "/res/" << "materials";
            hstring materialsPath = ss.str();

            {
                auto& watch = watcher.AddWatch(materialsPath.c_str());
                watch.FileModifiedEvents.AddEvent(nullptr, MaterialFileModified, 1);
                watch.FileAddedEvents.AddEvent(nullptr, MaterialFileModified, 1);
                watch.FileDeletedEvents.AddEvent(nullptr, MaterialFileModified, 1);
            }

            ss = {};
            ss << "workspace/" << title << "/res/" << "skybox";
            hstring skyboxPath = ss.str();

            {
                auto& watch = watcher.AddWatch(skyboxPath.c_str());
                watch.FileModifiedEvents.AddEvent(nullptr, SkyboxFileModified, 1);
                watch.FileAddedEvents.AddEvent(nullptr, SkyboxFileModified, 1);
                watch.FileDeletedEvents.AddEvent(nullptr, SkyboxFileModified, 1);
            }
        }

        hstring cProjectManager::GetProjectPath(const string &title)
        {
            hstringstream ss;
            ss << "workspace/" << title;
            return ss.str();
        }

        hstring cProjectManager::GetVSBuildPath(const string &title)
        {
            hstringstream ss;
            ss << "workspace/" << title << "/build";
            return ss.str();
        }

        hstring cProjectManager::GetLaunchPath(const string &title, sProject::eBuildType buildType)
        {
            hstringstream ss;
            ss << "workspace/" << title << "/" << s_BinPathTable.at(buildType);
            return ss.str();
        }

        void cProjectManager::LoadGameCode(const string &dllpath)
        {
            cApp* game = cProcessManager::LoadObjectFromDLL<cApp>(dllpath.c_str(), "CreateGame");
            if (game == nullptr) {
                LogError("Failed to load game code from DLL {}", dllpath);
                return;
            }
            // dispatch Game object with atomic thread-safe approach
            if (Callback != nullptr) {
                Callback->GameReloaded.store(game);
            }
        }

        void cProjectManager::OpenInСMake(const string &title)
        {
            std::thread thread([title]() {
                hstringstream ss;

                hstring projectPath = GetProjectPath(title);
                hstring buildPath = GetVSBuildPath(title);
                if (!cFileManager::Exists(projectPath.c_str())) {
                    LogError("Failed to open in CMake GUI for a game project {}. Path {} not found", title, projectPath);
                    return;
                }

                ss << "cmake-gui";
                ss << " -B " << cFileManager::GetAbsolutePath(buildPath.c_str());
                ss << " -S " << cFileManager::GetAbsolutePath(projectPath.c_str());
                system(ss.str().c_str());
            });
            thread.detach();
        }

        void cProjectManager::OpenInCLion(const string &title)
        {
            std::thread thread([title]() {
                hstringstream ss;

                hstring projectPath = GetProjectPath(title);
                if (!cFileManager::Exists(projectPath.c_str())) {
                    LogError("Failed to open in CLion IDE a game project {}. Path {} not found", title, projectPath);
                    return;
                }

                ss << "clion64.exe " << cFileManager::GetAbsolutePath(projectPath.c_str());
                system(ss.str().c_str());
            });
            thread.detach();
        }

        void cProjectManager::OpenInVS(const string &title)
        {
            std::thread thread([title]() {
                hstringstream ss;

                hstring projectPath = GetProjectPath(title);
                if (!cFileManager::Exists(projectPath.c_str())) {
                    LogError("Failed to open in Visual Studio IDE a game project {}. Path {} not found", title, projectPath);
                    return;
                }

                ss << "start devenv " << cFileManager::GetAbsolutePath(projectPath.c_str());
                system(ss.str().c_str());
            });
            thread.detach();
        }

        void cProjectManager::OpenInVSCode(const string &title)
        {
            std::thread thread([title]() {
                hstringstream ss;

                hstring projectPath = GetProjectPath(title);
                if (!cFileManager::Exists(projectPath.c_str())) {
                    LogError("Failed to open in VS Code a game project {}. Path {} not found", title, projectPath);
                    return;
                }

                ss << "code " << cFileManager::GetAbsolutePath(projectPath.c_str());
                system(ss.str().c_str());
            });
            thread.detach();
        }

        void cProjectManager::LaunchGame(const string &title, sProject::eBuildType buildType)
        {
            std::thread thread([title, buildType]() {
                hstringstream ss;

                hstring launchPath = GetLaunchPath(title, buildType);
                ss << launchPath << "/" << title << ".exe";
                hstring exePath = ss.str();

                if (!cFileManager::Exists(exePath.c_str())) {
                    LogError("Failed to launch a game executable of {}. Path {} not found", title, exePath);
                    return;
                }

                cProcessManager::StartExe(cFileManager::GetAbsolutePath(exePath.c_str()).c_str());
            });
            thread.detach();
        }

        void cProjectManager::StartWatching(const string& title)
        {
            auto project = s_Storage->Projects.find(title);

            if (project == s_Storage->Projects.end()) {
                LogError("Failed to start watching for game project {}. Project is absent!", title);
                return;
            }

            project->second.FilesWatcher->Start();
            project->second.DirsWatcher->Start();
        }

        void cProjectManager::StopWatching(const string& title)
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

}
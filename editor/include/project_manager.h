#pragma once

#include <os/file_watcher.h>

namespace xpe::core {
    class Application;
}

namespace focus {

    using namespace xpe::core;
    using namespace os;

    struct Project final {

        enum class eBuildType
        {
            DEBUG_VS,
            RELEASE_VS,
            DEBUG_CLION,
            RELEASE_CLION
        };

        string Title;
        MultiFileWatcher* FilesWatcher = nullptr;
        DirectoryWatcher* DirsWatcher = nullptr;
    };

    struct ProjectStorage : public Object {
        unordered_map<string, Project> Projects;
    };

    struct ProjectManagerCallback {
        std::atomic<Application*> GameReloaded;
    };

    class ProjectManager final {

    public:
        static ProjectManagerCallback* Callback;

    public:
        static void Init();
        static void Free();

        static Project* CreateProject(const string& title);

        static void StartWatching(const string& title);
        static void StopWatching(const string& title);

        static void LoadGameCode(const string& dllpath);

        static void OpenInСMake(const string& title);
        static void OpenInCLion(const string& title);
        static void OpenInVS(const string& title);
        static void OpenInVSCode(const string& title);

        static void LaunchGame(const string& title, Project::eBuildType buildType);

    private:
        static hstring GetProjectPath(const string& title);
        static hstring GetVSBuildPath(const string& title);
        static hstring GetLaunchPath(const string& title, Project::eBuildType buildType);

        static void SetupProject(const string& title);

        static void WatchProjectGameCodeDLL(MultiFileWatcher& watcher, const string& title, const char* buildPath);
        static void WatchProjectShaders(DirectoryWatcher& watcher, const string& title);
        static void WatchProjectResources(DirectoryWatcher& watcher, const string& title);

    private:
        static ProjectStorage* s_Storage;

    };

}
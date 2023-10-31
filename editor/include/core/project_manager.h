#pragma once

#include <core/file_watcher.h>

namespace xpe::core {
    class cApp;
}

namespace focus {

    using namespace xpe::core;

    namespace core {

        struct sProject final {

            enum class eBuildType
            {
                DEBUG_VS,
                RELEASE_VS,
                DEBUG_CLION,
                RELEASE_CLION
            };

            string Title;
            cMultiFileWatcher* FilesWatcher = nullptr;
            cDirectoryWatcher* DirsWatcher = nullptr;
        };

        struct sProjectStorage : public cObject {
            unordered_map<string, sProject> Projects;
        };

        struct sProjectManagerCallback {
            std::atomic<cApp*> GameReloaded;
        };

        class cProjectManager final {

        public:
            static sProjectManagerCallback* Callback;

        public:
            static void Init();
            static void Free();

            static sProject* CreateProject(const string& title);

            static void StartWatching(const string& title);
            static void StopWatching(const string& title);

            static void LoadGameCode(const string& dllpath);

            static void OpenInСMake(const string& title);
            static void OpenInCLion(const string& title);
            static void OpenInVS(const string& title);
            static void OpenInVSCode(const string& title);

            static void LaunchGame(const string& title, sProject::eBuildType buildType);

        private:
            static hstring GetProjectPath(const string& title);
            static hstring GetVSBuildPath(const string& title);
            static hstring GetLaunchPath(const string& title, sProject::eBuildType buildType);

            static void SetupProject(const string& title);

            static void WatchProjectGameCodeDLL(cMultiFileWatcher& watcher, const string& title, const char* buildPath);
            static void WatchProjectShaders(cDirectoryWatcher& watcher, const string& title);
            static void WatchProjectResources(cDirectoryWatcher& watcher, const string& title);

        private:
            static sProjectStorage* s_Storage;

        };

    }

}
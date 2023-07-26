#pragma once

namespace xpe::core {
    class Game;
}

namespace focus {

    using namespace xpe::core;
    using namespace xpe::os;

    struct Project final {
        string Title;
    };

    struct ProjectStorage : public Object {
        unordered_map<string, Project> Projects;
    };

    class ProjectManagerCallback {

    public:
        virtual void GameCodeReloaded(Game* game) = 0;

    };

    class ProjectManager final {

    public:
        static ProjectManagerCallback* Callback;

    public:
        static void Init();
        static void Free();

        static Project* CreateProject(const string& title);

        static void UpdateWatches();

        static void LoadGameCode(const string& dllpath);

    private:
        static void SetupProject(const string& title);
        static hstring GetProjectPath(const string& title);

        static void WatchProjectGameCodeDLL(MultiFileWatcher& watcher, const string& title, const char* buildPath);
        static void WatchProjectShaders(DirectoryWatcher& watcher, const string& title);
        static void WatchProjectResources(DirectoryWatcher& watcher, const string& title);

    private:
        static ProjectStorage* s_Storage;

    };

}
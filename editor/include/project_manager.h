#pragma once

namespace focus {

    using namespace xpe::core;

    struct Project final {
        string Title;
        string Workspace = "workspace";
    };

    struct ProjectStorage : public Object {
        unordered_map<string, Project> Projects;
    };

    class ProjectManager final {

    public:
        static void Init();
        static void Free();

        static Project* CreateProject(const string& title);

    private:
        static ProjectStorage* s_Storage;

    };

}
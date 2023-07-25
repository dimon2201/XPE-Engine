#include <project_manager.h>

namespace focus {

    using namespace xpe::os;

    ProjectStorage* ProjectManager::s_Storage = nullptr;

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
        if (s_Storage->Projects.find(title) != s_Storage->Projects.end()) {
            LogError("Failed to create project {}. Project already exists.", title);
            return &s_Storage->Projects[title];
        }

        // create path to project
        hstringstream ss;
        ss << "workspace" << "/" << title;
        hstring projectPath = ss.str();

        // create project folder
        FileManager::CreateDirectory(projectPath.c_str());

        // copy all from template to new project folder
        FileManager::CopyDirs("templates/Game", projectPath.c_str());

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

        Project project;
        project.Title = title;
        project.Workspace = "workspace";

        s_Storage->Projects[title] = project;

        return &s_Storage->Projects[title];
    }

}
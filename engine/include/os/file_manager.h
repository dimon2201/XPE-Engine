#pragma once

#include <os/file_watcher.h>

namespace xpe {

    namespace os {

        using namespace core;

        class ENGINE_API FileManager final {

        public:
            static string ReadFileWithIncludes(const string& filepath, string includeIdentifier = "#include");

            static string ReadFile(const char* filepath);

            static bool WriteFile(const char* filepath, const string& str);
            static bool WriteFile(const char* filepath, const char* bytes, usize byteSize);

            static bool CreateDir(const char* dirName);

            static void CopyDirs(const char* srcPath, const char* destPath, const bool createRoot = false);

            static bool Exists(const char* filepath);

            static DirectoryWatcher* CreateDirectoryWatch(const string& usid);
            static void RemoveDirectoryWatch(const string& usid);
            static void UpdateDirectoryWatchers();

            static MultiFileWatcher* CreateMultiFileWatch(const string& usid);
            static void RemoveMultiFileWatch(const string& usid);
            static void UpdateMultiFileWatchers();

        private:
            static void GetFilepath(const string& fullPath, string& pathWithoutFilename);

        private:
            static std::unordered_map<string, DirectoryWatcher> s_DirectoryWatchers;
            static std::unordered_map<string, MultiFileWatcher> s_MultiFileWatchers;

        };

    }

}
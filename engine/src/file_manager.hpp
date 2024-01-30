#pragma once

#include <event_manager.hpp>

namespace xpe
{
    namespace core
    {
        class cFileWatcher;

        struct sFileWatch final
        {
            bool DeleteNotified = false;
            std::filesystem::file_time_type Timestamp;
            cEventBuffer<const string&, const string&> FileAddedEvents;
            cEventBuffer<const string&, const string&> FileDeletedEvents;
            cEventBuffer<const string&, const string&> FileModifiedEvents;
            cEventBuffer<const string&, const string&> FileNewNameEvents;
            cEventBuffer<const string&, const string&> FileOldNameEvents;
        };

        class cFileWatcher : public cObject
        {

        public:
            enum class eAction
            {
                ADDED,
                DELETED,
                MODIFIED,
                NEW_NAME,
                OLD_NAME
            };

        protected:
            void Notify(
                    sFileWatch& watch,
                    const string& watchpath,
                    const string& filepath,
                    cFileWatcher::eAction action
            );
        };

        class cMultiFileWatcher : public cFileWatcher
        {

        public:
            sFileWatch& AddWatch(const char* path);
            void RemoveWatch(const char* path);

            void Start();
            void Stop();

        private:
            void Update();
            bool Exists(const string& path);

            bool m_Running = false;
            unordered_map<string, sFileWatch> m_Paths;
        };

        class cDirectoryWatcher : public cFileWatcher
        {

        public:
            sFileWatch& AddWatch(const char* path);
            void RemoveWatch(const char* path);

            void Start();
            void Stop();

        private:
            bool Exists(const string& watchpath);
            bool Exists(const string& watchpath, const string& filepath);
            void Update(const string& watchpath, const string& filepath, sFileWatch& watch);

            bool m_Running = false;
            unordered_map<string, unordered_map<string, std::filesystem::file_time_type>> m_Paths;
            unordered_map<string, sFileWatch> m_Watches;
        };

        namespace fs = std::filesystem;

        class ENGINE_API MFile final
        {

        public:
            static string ReadFileWithIncludes(const string& filepath, string includeIdentifier = "#include");

            static string ReadFile(const char* filepath);

            static bool WriteFile(const char* filepath, const string& str);
            static bool WriteFile(const char* filepath, const char* bytes, usize byteSize);

            static bool CreateDir(const char* dirName);

            static void CopyFile(const char* srcPath, const char* destPath);
            static void CopyDirs(const char* srcPath, const char* destPath, const bool createRoot = false);

            static hstring GetFullFileName(const char* path);
            static hstring GetFileName(const char* path);

            static bool Exists(const char* filepath);
            static bool NotExists(const char* filepath);

            static string GetAbsolutePath(const char* path);

            static hstring GetDirectory(const char* filepath);

        private:
            static void GetFilepath(const string& fullPath, string& pathWithoutFilename);

        };
    }
}
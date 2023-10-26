#pragma once

#include <core/event.h>

namespace focus {

    namespace core {

        using namespace xpe::core;

        typedef void (*FileAddedFn)(void* thiz, const string& watchpath, const string& filepath);
        template<typename T>
        static void OnFileAdded(void* const thiz, const string& watchpath, const string& filepath) {
            ((T*) thiz)->FileAdded(watchpath, filepath);
        }

        typedef void (*FileDeletedFn)(void* thiz, const string& watchpath, const string& filepath);
        template<typename T>
        static void OnFileDeleted(void* const thiz, const string& watchpath, const string& filepath) {
            ((T*) thiz)->FileDeleted(watchpath, filepath);
        }

        typedef void (*FileModifiedFn)(void* thiz, const string& watchpath, const string& filepath);
        template<typename T>
        static void OnFileModified(void* const thiz, const string& watchpath, const string& filepath) {
            ((T*) thiz)->FileModified(watchpath, filepath);
        }

        typedef void (*FileNewNameFn)(void* thiz, const string& watchpath, const string& filepath);
        template<typename T>
        static void OnFileNewName(void* const thiz, const string& watchpath, const string& filepath) {
            ((T*) thiz)->FileNewName(watchpath, filepath);
        }

        typedef void (*FileOldNameFn)(void* thiz, const string& watchpath, const string& filepath);
        template<typename T>
        static void OnFileOldName(void* const thiz, const string& watchpath, const string& filepath) {
            ((T*) thiz)->FileOldName(watchpath, filepath);
        }

        class FileWatcher;

        struct FileWatch final {
            bool DeleteNotified = false;
            std::filesystem::file_time_type Timestamp;
            EventBuffer<FileAddedFn> FileAddedEventBuffer;
            EventBuffer<FileDeletedFn> FileDeletedEventBuffer;
            EventBuffer<FileModifiedFn> FileModifiedEventBuffer;
            EventBuffer<FileNewNameFn> FileNewNameEventBuffer;
            EventBuffer<FileOldNameFn> FileOldNameEventBuffer;
        };

        struct FileWatcher : public Object
        {

            enum class eAction
            {
                ADDED,
                DELETED,
                MODIFIED,
                NEW_NAME,
                OLD_NAME
            };

            string USID;

            FileWatcher(const string& usid) : USID(usid) {}

        protected:
            void Notify(
                    FileWatch& watch,
                    const string& watchpath,
                    const string& filepath,
                    FileWatcher::eAction action
            );
        };

        struct MultiFileWatcher : public FileWatcher
        {

            MultiFileWatcher(const string& usid) : FileWatcher(usid) {}

            FileWatch& AddWatch(const char* path);
            void RemoveWatch(const char* path);

            void Start();
            void Stop();

        private:
            void Update();

            bool Exists(const string& path);

        private:
            bool m_Running = false;
            unordered_map<string, FileWatch> m_Paths;
        };

        struct DirectoryWatcher : public FileWatcher
        {

            DirectoryWatcher(const string& usid) : FileWatcher(usid) {}

            FileWatch& AddWatch(const char* path);
            void RemoveWatch(const char* path);

            void Start();
            void Stop();

        private:
            bool Exists(const string& watchpath);
            bool Exists(const string& watchpath, const string& filepath);

            void Update(const string& watchpath, const string& filepath, FileWatch& watch);

        private:
            bool m_Running = false;
            unordered_map<string, unordered_map<string, std::filesystem::file_time_type>> m_Paths;
            unordered_map<string, FileWatch> m_Watches;
        };

    }

}
#pragma once

#include <core/events.h>

namespace focus {

    namespace core {

        using namespace xpe::core;

        class cFileWatcher;

        struct sFileWatch final {
            bool DeleteNotified = false;
            std::filesystem::file_time_type Timestamp;
            cEventBuffer<fEventFileAdded> FileAddedEvents;
            cEventBuffer<fEventFileDeleted> FileDeletedEvents;
            cEventBuffer<fEventFileModified> FileModifiedEvents;
            cEventBuffer<fEventFileNewName> FileNewNameEvents;
            cEventBuffer<fEventFileOldName> FileOldNameEvents;
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

            string USID;

            cFileWatcher(const string& usid) : USID(usid) {}

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
            cMultiFileWatcher(const string& usid) : cFileWatcher(usid) {}

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
            cDirectoryWatcher(const string& usid) : cFileWatcher(usid) {}

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

    }

}
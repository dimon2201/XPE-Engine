#include <core/file_watcher.hpp>

namespace focus {

    namespace core {

        void cFileWatcher::Notify(sFileWatch& watch, const string &watchpath, const string &filepath, cFileWatcher::eAction action)
        {
            switch (action) {

                case cFileWatcher::eAction::OLD_NAME:
                    watch.FileOldNameEvents.NotifyAll(watchpath, filepath);
                    break;

                case cFileWatcher::eAction::NEW_NAME:
                    watch.FileNewNameEvents.NotifyAll(watchpath, filepath);
                    break;

                case cFileWatcher::eAction::ADDED:
                    watch.FileAddedEvents.NotifyAll(watchpath, filepath);
                    break;

                case cFileWatcher::eAction::DELETED:
                    watch.FileDeletedEvents.NotifyAll(watchpath, filepath);
                    break;

                case cFileWatcher::eAction::MODIFIED:
                    watch.FileModifiedEvents.NotifyAll(watchpath, filepath);
                    break;

            }
        }

        sFileWatch& cMultiFileWatcher::AddWatch(const char* path)
        {
            m_Paths[path].Timestamp = std::filesystem::last_write_time(path);
            return m_Paths[path];
        }

        void cMultiFileWatcher::RemoveWatch(const char* path)
        {
            m_Paths.erase(path);
        }

        bool cMultiFileWatcher::Exists(const string &path)
        {
            return m_Paths.find(path) != m_Paths.end();
        }

        void cMultiFileWatcher::Start()
        {
            m_Running = true;
            cTaskManager::SubmitTask({[this]() {
                while (m_Running) {
                    Update();
                }
            }});
        }

        void cMultiFileWatcher::Stop()
        {
            m_Running = false;
        }

        void cMultiFileWatcher::Update()
        {
            for (auto& watchpath : m_Paths)
            {
                auto& filepath = (string&)(watchpath.first);
                sFileWatch& filewatch = m_Paths[filepath];

                // check if file was deleted
                if (!std::filesystem::exists(filepath)) {
                    if (!filewatch.DeleteNotified) {
                        Notify(filewatch, filepath, filepath, eAction::DELETED);
                        filewatch.DeleteNotified = true;
                    }
                    // we don't want to remove watch path even if it's deleted.
                    // because it could be deleted for a short time or replaced.
                    // so we want to continue watching.
                    continue;
                }

                filewatch.DeleteNotified = false;
                auto currentFileLastWriteTime = std::filesystem::last_write_time(filepath.c_str());

                // check if file was added
                if (!Exists(filepath)) {
                    filewatch.Timestamp = currentFileLastWriteTime;
                    Notify(filewatch, filepath, filepath, eAction::ADDED);
                }

                // check if file was modified
                else if (filewatch.Timestamp != currentFileLastWriteTime) {
                    filewatch.Timestamp = currentFileLastWriteTime;
                    Notify(filewatch, filepath, filepath, eAction::MODIFIED);
                }
            }
        }

        sFileWatch& cDirectoryWatcher::AddWatch(const char* path)
        {
            m_Paths[path] = {};

            for (auto &file : std::filesystem::recursive_directory_iterator(path))
            {
                m_Paths[path][file.path().string().c_str()] = std::filesystem::last_write_time(file);
            }

            return m_Watches[path];
        }

        void cDirectoryWatcher::RemoveWatch(const char* path)
        {
            m_Paths.erase(path);
        }

        bool cDirectoryWatcher::Exists(const string &watchpath)
        {
            return m_Paths.find(watchpath) != m_Paths.end();
        }

        bool cDirectoryWatcher::Exists(const string &watchpath, const string& filepath)
        {
            auto& files = m_Paths[watchpath];
            return files.find(filepath) != files.end();
        }

        void cDirectoryWatcher::Start()
        {
            m_Running = true;
            cTaskManager::SubmitTask({[this]() {
                while (m_Running) {
                    for (auto& watchpath : m_Paths)
                    {
                        auto& watch = m_Watches[watchpath.first];
                        for (auto& filepath : watchpath.second) {
                            Update(watchpath.first, filepath.first, watch);
                        }
                    }
                }
            }});
        }

        void cDirectoryWatcher::Stop()
        {
            m_Running = false;
        }

        void cDirectoryWatcher::Update(const string& watchpath, const string& filepath, sFileWatch& watch)
        {
            auto& files = m_Paths[watchpath];

            // check if a file was deleted
            if (!std::filesystem::exists(filepath)) {
                Notify(watch, watchpath, filepath, eAction::DELETED);
                files.erase(filepath);
            }

            // check if a file was added or modified
            for (auto &file : std::filesystem::recursive_directory_iterator(watchpath))
            {
                auto currentFileLastWriteTime = std::filesystem::last_write_time(file);
                string fsfilepath = file.path().string().c_str();

                // check if file was added
                if (!Exists(watchpath, fsfilepath)) {
                    files.insert({ fsfilepath, currentFileLastWriteTime });
                    Notify(watch, watchpath, fsfilepath, eAction::ADDED);
                    continue;
                }

                // check if file was modified
                auto& timestamp = files[fsfilepath];
                if (timestamp != currentFileLastWriteTime) {
                    timestamp = currentFileLastWriteTime;
                    Notify(watch, watchpath, fsfilepath, eAction::MODIFIED);
                }
            }
        }

    }

}
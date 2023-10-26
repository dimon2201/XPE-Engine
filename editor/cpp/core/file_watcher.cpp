#include <core/file_watcher.h>

namespace focus {

    namespace core {

        void FileWatcher::Notify(FileWatch& watch, const string &watchpath, const string &filepath, FileWatcher::eAction action)
        {
            switch (action) {

                case FileWatcher::eAction::OLD_NAME:
                    watch.FileOldNameEventBuffer.NotifyAll(watchpath, filepath);
                    break;

                case FileWatcher::eAction::NEW_NAME:
                    watch.FileNewNameEventBuffer.NotifyAll(watchpath, filepath);
                    break;

                case FileWatcher::eAction::ADDED:
                    watch.FileAddedEventBuffer.NotifyAll(watchpath, filepath);
                    break;

                case FileWatcher::eAction::DELETED:
                    watch.FileDeletedEventBuffer.NotifyAll(watchpath, filepath);
                    break;

                case FileWatcher::eAction::MODIFIED:
                    watch.FileModifiedEventBuffer.NotifyAll(watchpath, filepath);
                    break;

            }
        }

        FileWatch& MultiFileWatcher::AddWatch(const char* path)
        {
            m_Paths[path].Timestamp = std::filesystem::last_write_time(path);
            return m_Paths[path];
        }

        void MultiFileWatcher::RemoveWatch(const char* path)
        {
            m_Paths.erase(path);
        }

        bool MultiFileWatcher::Exists(const string &path)
        {
            return m_Paths.find(path) != m_Paths.end();
        }

        void MultiFileWatcher::Start()
        {
            m_Running = true;
            TaskManager::SubmitTask({[this]() {
                while (m_Running) {
                    Update();
                }
            }});
        }

        void MultiFileWatcher::Stop()
        {
            m_Running = false;
        }

        void MultiFileWatcher::Update()
        {
            for (auto& watchpath : m_Paths)
            {
                auto& filepath = (string&)(watchpath.first);
                FileWatch& filewatch = m_Paths[filepath];

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

        FileWatch& DirectoryWatcher::AddWatch(const char* path)
        {
            m_Paths[path] = {};

            for (auto &file : std::filesystem::recursive_directory_iterator(path))
            {
                m_Paths[path][file.path().string().c_str()] = std::filesystem::last_write_time(file);
            }

            return m_Watches[path];
        }

        void DirectoryWatcher::RemoveWatch(const char* path)
        {
            m_Paths.erase(path);
        }

        bool DirectoryWatcher::Exists(const string &watchpath)
        {
            return m_Paths.find(watchpath) != m_Paths.end();
        }

        bool DirectoryWatcher::Exists(const string &watchpath, const string& filepath)
        {
            auto& files = m_Paths[watchpath];
            return files.find(filepath) != files.end();
        }

        void DirectoryWatcher::Start()
        {
            m_Running = true;
            TaskManager::SubmitTask({[this]() {
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

        void DirectoryWatcher::Stop()
        {
            m_Running = false;
        }

        void DirectoryWatcher::Update(const string& watchpath, const string& filepath, FileWatch& watch)
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
#include <os/file_watcher.h>

namespace xpe {

    namespace os {

        void FileWatcher::Dispatch(FileWatch& watch, const string &watchpath, const string &filepath, FileWatcher::eAction action)
        {
            switch (action) {

                case FileWatcher::eAction::OLD_NAME:
                    {
                        auto& event = watch.FileOldNameEvent;
                        if (event.Thiz != nullptr || event.Function != nullptr) {
                            event.Function(event.Thiz, watchpath, filepath);
                        }
                    }
                    break;

                case FileWatcher::eAction::NEW_NAME:
                    {
                        auto& event = watch.FileNewNameEvent;
                        if (event.Thiz != nullptr || event.Function != nullptr) {
                            event.Function(event.Thiz, watchpath, filepath);
                        }
                    }
                    break;

                case FileWatcher::eAction::ADDED:
                    {
                        auto& event = watch.FileAddedEvent;
                        if (event.Thiz != nullptr || event.Function != nullptr) {
                            event.Function(event.Thiz, watchpath, filepath);
                        }
                    }
                    break;

                case FileWatcher::eAction::DELETED:
                    {
                        auto& event = watch.FileDeletedEvent;
                        if (event.Thiz != nullptr || event.Function != nullptr) {
                            event.Function(event.Thiz, watchpath, filepath);
                        }
                    }
                    break;

                case FileWatcher::eAction::MODIFIED:
                    {
                        auto& event = watch.FileModifiedEvent;
                        if (event.Thiz != nullptr || event.Function != nullptr) {
                            event.Function(event.Thiz, watchpath, filepath);
                        }
                    }
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

        void MultiFileWatcher::Update()
        {
            auto it = m_Paths.begin();

            // check if a file was deleted
            while (it != m_Paths.end())
            {
                if (!std::filesystem::exists(it->first)) {
                    Dispatch(it->second, it->first, it->first, eAction::DELETED);
                    it = m_Paths.erase(it);
                }

                else {
                    it++;
                }
            }

            // check if a file was added or modified
            for (auto& watchpath : m_Paths)
            {
                string filepath = watchpath.first;
                FileWatch& filewatch = m_Paths[filepath];
                auto currentFileLastWriteTime = std::filesystem::last_write_time(filepath);

                // check if file was added
                if (!Exists(filepath)) {
                    filewatch.Timestamp = currentFileLastWriteTime;
                    Dispatch(filewatch, filepath, filepath, eAction::ADDED);
                }

                // check if file was modified
                else if (filewatch.Timestamp != currentFileLastWriteTime) {
                    filewatch.Timestamp = currentFileLastWriteTime;
                    Dispatch(filewatch, filepath, filepath, eAction::MODIFIED);
                }
            }
        }

        FileWatch& DirectoryWatcher::AddWatch(const char* path)
        {
            m_Paths[path] = {};
            m_Watches[path] = {};
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

        void DirectoryWatcher::Update()
        {
            for (auto& watchpath : m_Paths)
            {
                auto& watch = m_Watches[watchpath.first];
                for (auto& filepath : watchpath.second) {
                    Update(watchpath.first, filepath.first, watch);
                }
            }
        }

        void DirectoryWatcher::Update(const string& watchpath, const string& filepath, FileWatch& watch)
        {
            auto& files = m_Paths[watchpath];

            // check if a file was deleted
            if (!std::filesystem::exists(filepath)) {
                Dispatch(watch, watchpath, filepath, eAction::DELETED);
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
                    Dispatch(watch, watchpath, fsfilepath, eAction::ADDED);
                    continue;
                }

                // check if file was modified
                auto& timestamp = files[fsfilepath];
                if (timestamp != currentFileLastWriteTime) {
                    timestamp = currentFileLastWriteTime;
                    Dispatch(watch, watchpath, fsfilepath, eAction::MODIFIED);
                }
            }
        }

    }

}
#include <file_manager.hpp>

namespace xpe
{
    namespace core
    {
        void cFileWatcher::Notify(sFileWatch& watch, const string &watchpath, const string &filepath, cFileWatcher::eAction action)
        {
            switch (action)
            {
                case cFileWatcher::eAction::OLD_NAME:
                    watch.FileOldNameEvents.Notify(watchpath, filepath);
                    break;

                case cFileWatcher::eAction::NEW_NAME:
                    watch.FileNewNameEvents.Notify(watchpath, filepath);
                    break;

                case cFileWatcher::eAction::ADDED:
                    watch.FileAddedEvents.Notify(watchpath, filepath);
                    break;

                case cFileWatcher::eAction::DELETED:
                    watch.FileDeletedEvents.Notify(watchpath, filepath);
                    break;

                case cFileWatcher::eAction::MODIFIED:
                    watch.FileModifiedEvents.Notify(watchpath, filepath);
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
            MTask::Add({sTask::eType::THREAD_POOL, [this]() {
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
                if (!std::filesystem::exists(filepath))
                {
                    if (!filewatch.DeleteNotified)
                    {
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
                if (!Exists(filepath))
                {
                    filewatch.Timestamp = currentFileLastWriteTime;
                    Notify(filewatch, filepath, filepath, eAction::ADDED);
                }

                // check if file was modified
                else if (filewatch.Timestamp != currentFileLastWriteTime)
                {
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
            MTask::Add({sTask::eType::THREAD_POOL, [this]() {
                while (m_Running) {
                    for (auto &watchpath: m_Paths) {
                        auto &watch = m_Watches[watchpath.first];
                        for (auto &filepath: watchpath.second) {
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
            if (!std::filesystem::exists(filepath))
            {
                Notify(watch, watchpath, filepath, eAction::DELETED);
                files.erase(filepath);
            }

            // check if a file was added or modified
            for (auto &file : std::filesystem::recursive_directory_iterator(watchpath))
            {
                auto currentFileLastWriteTime = std::filesystem::last_write_time(file);
                string fsfilepath = file.path().string().c_str();

                // check if file was added
                if (!Exists(watchpath, fsfilepath))
                {
                    files.insert({ fsfilepath, currentFileLastWriteTime });
                    Notify(watch, watchpath, fsfilepath, eAction::ADDED);
                    continue;
                }

                // check if file was modified
                auto& timestamp = files[fsfilepath];
                if (timestamp != currentFileLastWriteTime)
                {
                    timestamp = currentFileLastWriteTime;
                    Notify(watch, watchpath, fsfilepath, eAction::MODIFIED);
                }
            }
        }

        bool MFile::CreateDir(const char* dirName)
        {
            return fs::create_directory(dirName);
        }

        string MFile::ReadFileWithIncludes(const string& path, string includeIdentifier)
        {
            includeIdentifier += ' ';
            static bool recursive = false;
            string fullSrc = "";
            std::ifstream file;

            file.open(path.c_str());

            if (!file.is_open())
            {
                LogError("Failed to open file {}", path.c_str());
                return fullSrc;
            }

            string lineBuffer;
            while (std::getline(file, lineBuffer))
            {
                // Look for the new mShader include identifier
                if (lineBuffer.find(includeIdentifier) != lineBuffer.npos)
                {
                    // Remove the include identifier, this will cause the path to remain
                    lineBuffer.erase(0, includeIdentifier.size());

                    // The include path is relative to the current mShader file path
                    string pathOfThisFile;
                    GetFilepath(path, pathOfThisFile);
                    lineBuffer.insert(0, pathOfThisFile);

                    // By using recursion, the new include file can be extracted
                    // and inserted at this location in the mShader source code
                    recursive = true;
                    fullSrc += ReadFileWithIncludes(lineBuffer, includeIdentifier) + '\n';

                    // Do not add this line to the mShader source code, as the include
                    // path would generate a compilation issue in the final source code
                    continue;
                }

                fullSrc += lineBuffer + '\n';
            }

            // Only add the null terminator at the end of the complete file,
            // essentially skipping recursive function calls this way
            if (!recursive)
            {
                fullSrc += '\0';
            }

            file.close();

            return fullSrc;
        }

        void MFile::GetFilepath(const string& fullPath, string& pathWithoutFilename)
        {
            size_t found = fullPath.find_last_of("/\\");
            pathWithoutFilename = fullPath.substr(0, found + 1);
        }

        string MFile::ReadFile(const char* filepath)
        {
            std::ifstream file;

            file.open(filepath);

            if (!file.is_open())
            {
                LogError("Failed to open-read a file {}", filepath);
                return {};
            }

            string line;
            string buffer;
            while (std::getline(file, line))
            {
                buffer += line + '\n';
            }

            file.close();

            return buffer;
        }

        bool MFile::WriteFile(const char* filepath, const string &str)
        {
            return WriteFile(filepath, str.c_str(), str.length());
        }

        bool MFile::WriteFile(const char* filepath, const char* bytes, usize byteSize)
        {
            std::ofstream file;

            file.open(filepath);

            if (!file.is_open())
            {
                LogError("Failed to open-write a file {}", filepath);
                return false;
            }

            file.write(bytes, byteSize);

            file.close();

            return true;
        }

        void MFile::CopyFile(const char *srcPath, const char *destPath)
        {
            if (!fs::exists(destPath))
            {
                WriteFile(destPath, "");
            }
            fs::copy_file(srcPath, destPath, fs::copy_options::overwrite_existing);
        }

        hstring MFile::GetFullFileName(const char *path)
        {
            fs::path filepath = path;
            if (filepath.has_filename())
            {
                return filepath.filename().string().c_str();
            }
            return {};
        }

        hstring MFile::GetFileName(const char* path)
        {
            fs::path filepath = path;
            if (filepath.has_filename())
            {
                return filepath.filename().string().c_str();
            }
            return {};
        }

        void MFile::CopyDirs(const char* srcPath, const char* destPath, const bool createRoot)
        {
            if (createRoot)
            {
                fs::create_directory(destPath);
            }

            for (fs::path p: fs::directory_iterator(srcPath))
            {
                fs::path destFile = destPath/p.filename();

                if (fs::is_directory(p))
                {
                    fs::create_directory(destFile);
                    CopyDirs(p.string().c_str(), destFile.string().c_str(), false);
                }

                else
                {
                    try
                    {
                        if (fs::exists(destFile))
                        {
                            fs::copy_file(p, destFile, fs::copy_options::overwrite_existing);
                        }

                        else
                        {
                            fs::copy(p, destFile);
                        }

                    }
                    catch (std::exception& e)
                    {
                        LogError("Failed to copy dirs from {} to {} \n Error: {}", p.string(), destFile.string(), e.what());
                    }
                }
            }
        }

        bool MFile::Exists(const char *filepath)
        {
            return fs::exists(filepath);
        }

        bool MFile::NotExists(const char *filepath)
        {
            return !fs::exists(filepath);
        }

        string MFile::GetAbsolutePath(const char* path)
        {
            return fs::absolute(path).string().c_str();
        }

        hstring MFile::GetDirectory(const char *filepath)
        {
            hstring filepathStr = filepath;
            return filepathStr.substr(0, filepathStr.find_last_of('/'));
        }

    }

}
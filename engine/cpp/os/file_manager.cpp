
#include <os/file_manager.h>

namespace xpe {

    namespace os {

        bool FileManager::CreateDir(const char* dirName) {
            return fs::create_directory(dirName);
        }

        string FileManager::ReadFileWithIncludes(const string& path, string includeIdentifier)
        {
            includeIdentifier += ' ';
            static bool recursive = false;
            string fullSrc = "";
            std::ifstream file;

            file.open(path.c_str());

            if (!file.is_open()) {
                LogError("Failed to open file {}", path.c_str());
                return fullSrc;
            }

            string lineBuffer;
            while (std::getline(file, lineBuffer)) {
                // Look for the new mShader include identifier
                if (lineBuffer.find(includeIdentifier) != lineBuffer.npos) {
                    // Remove the include identifier, this will cause the path to remain
                    lineBuffer.erase(0, includeIdentifier.size());

                    // The include path is relative to the current mShader file path
                    string pathOfThisFile;
                    GetFilepath(path, pathOfThisFile);
                    lineBuffer.insert(0, pathOfThisFile);

                    // By using recursion, the new include file can be extracted
                    // and inserted at this location in the mShader source code
                    recursive = true;
                    fullSrc += ReadFileWithIncludes(lineBuffer, includeIdentifier);

                    // Do not add this line to the mShader source code, as the include
                    // path would generate a compilation issue in the final source code
                    continue;
                }

                fullSrc += lineBuffer + '\n';
            }

            // Only add the null terminator at the end of the complete file,
            // essentially skipping recursive function calls this way
            if (!recursive)
                fullSrc += '\0';

            file.close();

            return fullSrc;
        }

        void FileManager::GetFilepath(
                const string& fullPath,
                string& pathWithoutFilename
        )
        {
            // Remove the file name and store the path to this folder
            size_t found = fullPath.find_last_of("/\\");
            pathWithoutFilename = fullPath.substr(0, found + 1);
        }

        string FileManager::ReadFile(const char* filepath)
        {
            std::ifstream file;

            file.open(filepath);

            if (!file.is_open()) {
                LogError("Failed to open-read a file {}", filepath);
                return {};
            }

            string line;
            string buffer;
            while (std::getline(file, line)) {
                buffer += line + '\n';
            }

            file.close();

            return buffer;
        }

        bool FileManager::WriteFile(const char* filepath, const string &str)
        {
            return WriteFile(filepath, str.c_str(), str.length());
        }

        bool FileManager::WriteFile(const char* filepath, const char* bytes, usize byteSize)
        {
            std::ofstream file;

            file.open(filepath);

            if (!file.is_open()) {
                LogError("Failed to open-write a file {}", filepath);
                return false;
            }

            file.write(bytes, byteSize);

            file.close();

            return true;
        }

        void FileManager::CopyFile(const char *srcPath, const char *destPath)
        {
            if (!fs::exists(destPath)) {
                WriteFile(destPath, "");
            }
            fs::copy_file(srcPath, destPath, fs::copy_options::overwrite_existing);
        }

        hstring FileManager::GetFullFileName(const char *path)
        {
            fs::path filepath = path;
            if (filepath.has_filename()) {
                return filepath.filename().string().c_str();
            }
            return {};
        }

        hstring FileManager::GetFileName(const char* path)
        {
            // todo remove extension from file name
            fs::path filepath = path;
            if (filepath.has_filename()) {
                return filepath.filename().string().c_str();
            }
            return {};
        }

        void FileManager::CopyDirs(const char* srcPath, const char* destPath, const bool createRoot)
        {
            if (createRoot) {
                fs::create_directory(destPath);
            }

            for (fs::path p: fs::directory_iterator(srcPath))
            {
                fs::path destFile = destPath/p.filename();

                if (fs::is_directory(p)) {
                    fs::create_directory(destFile);
                    CopyDirs(p.string().c_str(), destFile.string().c_str(), false);
                }

                else {

                    try {

                        if (fs::exists(destFile)) {
                            fs::copy_file(p, destFile, fs::copy_options::overwrite_existing);
                        }

                        else {
                            fs::copy(p, destFile);
                        }

                    } catch (std::exception& e) {
                        LogError("Failed to copy dirs from {} to {} \n Error: {}",
                                 p.string(),
                                 destFile.string(),
                                 e.what());
                    }

                }
            }
        }

        bool FileManager::Exists(const char *filepath)
        {
            return fs::exists(filepath);
        }

        string FileManager::GetAbsolutePath(const char* path)
        {
            return fs::absolute(path).string().c_str();
        }

    }

}
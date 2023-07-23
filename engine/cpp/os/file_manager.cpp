
#include <os/file_manager.h>

namespace xpe {

    namespace os {

        string FileManager::ReadFileWithIncludes(const string& path, string includeIdentifier) {
            includeIdentifier += ' ';
            static bool recursive = false;
            string fullSrc = "";
            std::ifstream file;

            file.open(path.c_str());

            if (!file.is_open()) {
                LogError("Failed to open file {0}", path.c_str());
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
        ) {
            // Remove the file name and store the path to this folder
            size_t found = fullPath.find_last_of("/\\");
            pathWithoutFilename = fullPath.substr(0, found + 1);
        }

        bool FileManager::WriteFile(const char* filepath, const string &str) {
            return WriteFile(filepath, str.c_str(), str.length());
        }

        bool FileManager::WriteFile(const char* filepath, const char* bytes, usize byteSize) {
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

    }

}
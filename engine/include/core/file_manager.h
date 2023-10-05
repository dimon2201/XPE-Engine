#pragma once

namespace xpe {

    namespace core {

        namespace fs = std::filesystem;

        class ENGINE_API FileManager final {

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

            static string GetAbsolutePath(const char* path);

            static hstring GetDirectory(const char* filepath);

        private:
            static void GetFilepath(const string& fullPath, string& pathWithoutFilename);

        };

    }

}
#pragma once

namespace xpe {

    namespace os {

        using namespace core;

        class ENGINE_API FileManager final {

        public:
            static string ReadFileWithIncludes(const string& filepath, string includeIdentifier = "#include");

            static string ReadFile(const char* filepath);

            static bool WriteFile(const char* filepath, const string& str);
            static bool WriteFile(const char* filepath, const char* bytes, usize byteSize);

            static bool CreateDirectory(const string& dirName);

            static void CopyDirs(const char* srcPath, const char* destPath, const bool createRoot = false);

        private:
            static void GetFilepath(const string& fullPath, string& pathWithoutFilename);

        };

    }

}
#pragma once

namespace xpe {

    namespace core {

        class ENGINE_API FileManager final {

        public:
            static string ReadFileWithIncludes(const string& filepath, string includeIdentifier = "#include");

        private:
            static void GetFilepath(const string& fullPath, string& pathWithoutFilename);

        };

    }

}
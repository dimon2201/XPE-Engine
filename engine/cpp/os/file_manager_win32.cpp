#ifdef WINDOWS

#include <direct.h>

namespace xpe {

    namespace os {

        bool FileManager::CreateDirectory(const string &dirName) {
            return _mkdir(dirName.c_str()) == 0;
        }

    }

}

#endif // WINDOWS
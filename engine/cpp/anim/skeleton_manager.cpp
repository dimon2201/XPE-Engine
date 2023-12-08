#include <anim/skeleton_manager.h>

namespace xpe {

    namespace anim {

        void cSkeletonManager::Init()
        {
            Buffers::Skeleton = new sSkeletonBuffer();
        }

        void cSkeletonManager::Free()
        {
            delete Buffers::Skeleton;
        }

        void cSkeletonManager::Flush()
        {
            Buffers::Skeleton->Flush();
        }

    }

}
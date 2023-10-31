#include <anim/skeleton_manager.h>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace anim {

        vector<render::sBoneBuffer*>* cSkeletonManager::s_Buffers = nullptr;

        void cSkeletonManager::Init()
        {
            s_Buffers = new vector<render::sBoneBuffer*>();
        }

        void cSkeletonManager::Free()
        {
            for (auto* buffer : *s_Buffers) {
                delete buffer;
            }
            delete s_Buffers;
        }

        void cSkeletonManager::Bind(u32 index)
        {
            render::context::BindVSBuffer(*s_Buffers->at(index));
        }

        void cSkeletonManager::Unbind(u32 index)
        {
            render::context::UnbindVSBuffer(*s_Buffers->at(index));
        }

        void cSkeletonManager::Flush(u32 index)
        {
            s_Buffers->at(index)->Flush();
        }

        void cSkeletonManager::RemoveSkeleton(u32 index)
        {
            delete s_Buffers->at(index);
            s_Buffers->at(index) = nullptr;
        }

        render::sBoneBuffer* cSkeletonManager::GetBuffer(u32 index)
        {
            return s_Buffers->at(index);
        }

    }

}
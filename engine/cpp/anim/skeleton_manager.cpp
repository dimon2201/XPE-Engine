#include <anim/skeleton_manager.h>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace anim {

        vector<render::BoneBuffer*>* mSkeletonManager::s_Buffers = nullptr;

        void mSkeletonManager::Init()
        {
            s_Buffers = new vector<render::BoneBuffer*>();
        }

        void mSkeletonManager::Free()
        {
            for (auto* buffer : *s_Buffers) {
                delete buffer;
            }
            delete s_Buffers;
        }

        void mSkeletonManager::Bind(u32 index)
        {
            render::context::BindVSBuffer(*s_Buffers->at(index));
        }

        void mSkeletonManager::Unbind(u32 index)
        {
            render::context::UnbindVSBuffer(*s_Buffers->at(index));
        }

        void mSkeletonManager::Flush(u32 index)
        {
            s_Buffers->at(index)->Flush();
        }

        void mSkeletonManager::RemoveSkeleton(u32 index)
        {
            delete s_Buffers->at(index);
            s_Buffers->at(index) = nullptr;
        }

        render::BoneBuffer* mSkeletonManager::GetBuffer(u32 index)
        {
            return s_Buffers->at(index);
        }

    }

}
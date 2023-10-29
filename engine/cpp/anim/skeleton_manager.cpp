#include <anim/skeleton_manager.h>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace anim {

        vector<render::BoneBuffer*>* SkeletonManager::s_Buffers = nullptr;

        void SkeletonManager::Init()
        {
            s_Buffers = new vector<render::BoneBuffer*>();
        }

        void SkeletonManager::Free()
        {
            for (auto* buffer : *s_Buffers) {
                delete buffer;
            }
            delete s_Buffers;
        }

        void SkeletonManager::Bind(u32 index)
        {
            render::context::BindVSBuffer(*s_Buffers->at(index));
        }

        void SkeletonManager::Unbind(u32 index)
        {
            render::context::UnbindVSBuffer(*s_Buffers->at(index));
        }

        void SkeletonManager::Flush(u32 index)
        {
            s_Buffers->at(index)->Flush();
        }

        void SkeletonManager::RemoveSkeleton(u32 index)
        {
            delete s_Buffers->at(index);
            s_Buffers->at(index) = nullptr;
        }

        render::BoneBuffer* SkeletonManager::GetBuffer(u32 index)
        {
            return s_Buffers->at(index);
        }

    }

}
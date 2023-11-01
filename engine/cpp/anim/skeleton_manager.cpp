#include <anim/skeleton_manager.h>

#include <rendering/core/context.hpp>

namespace xpe {

    namespace anim {

        render::sSkeletonBuffer* cSkeletonManager::s_Buffer = nullptr;

        void cSkeletonManager::Init()
        {
            s_Buffer = new render::sSkeletonBuffer();
        }

        void cSkeletonManager::Free()
        {
            delete s_Buffer;
        }

        void cSkeletonManager::Bind()
        {
            render::context::BindVSBuffer(*s_Buffer);
        }

        void cSkeletonManager::Unbind()
        {
            render::context::UnbindVSBuffer(*s_Buffer);
        }

        void cSkeletonManager::Flush()
        {
            s_Buffer->Flush();
        }

        render::sSkeletonBuffer* cSkeletonManager::GetBuffer()
        {
            return s_Buffer;
        }

    }

}
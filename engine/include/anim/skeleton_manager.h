#pragma once

#include <anim/skeleton.h>

namespace xpe {

    namespace anim {

        using namespace ecs;

        class ENGINE_API cSkeletonManager final
        {
        public:
            static void Init();
            static void Free();

            static void Bind();
            static void Unbind();
            static void Flush();

            template<typename... Args>
            static sSkeleton AddSkeleton(Args &&... args);

            static render::sSkeletonBuffer* GetBuffer();

        private:
            static render::sSkeletonBuffer* s_Buffer;
        };

        template<typename... Args>
        sSkeleton cSkeletonManager::AddSkeleton(Args &&... args)
        {
            sSkeleton skeleton(std::forward<Args>(args)...);
            skeleton.Index = s_Buffer->Size();
            s_Buffer->Resize(s_Buffer->Size() + skeleton.Bones.size());
            return skeleton;
        }

    }

}
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
            static std::pair<sSkeleton, sSkeletonInfo> AddSkeleton(Args &&... args);

            static render::sSkeletonBuffer* GetBuffer();

        private:
            static render::sSkeletonBuffer* s_Buffer;
        };

        template<typename... Args>
        std::pair<sSkeleton, sSkeletonInfo> cSkeletonManager::AddSkeleton(Args &&... args)
        {
            sSkeleton skeleton(std::forward<Args>(args)...);
            sSkeletonInfo skeletonInfo;
            skeletonInfo.SkeletonIndex = s_Buffer->Size();
            s_Buffer->Resize(s_Buffer->Size() + skeleton.Bones.size());
            return { skeleton, skeletonInfo };
        }

    }

}
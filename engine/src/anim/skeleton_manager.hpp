#pragma once

#include <anim/skeleton.hpp>
#include <rendering/bindings.hpp>

namespace xpe {

    namespace anim {

        using namespace ecs;
        using namespace render;

        class ENGINE_API cSkeletonManager final
        {
        public:
            static void Init();
            static void Free();
            static void Flush();

            template<typename... Args>
            static std::pair<sSkeleton, sSkeletonInfo> AddSkeleton(Args &&... args);
        };

        template<typename... Args>
        std::pair<sSkeleton, sSkeletonInfo> cSkeletonManager::AddSkeleton(Args &&... args)
        {
            sSkeleton skeleton(std::forward<Args>(args)...);
            sSkeletonInfo skeletonInfo;
            skeletonInfo.SkeletonIndex = Buffers::Skeleton->Size();
            Buffers::Skeleton->Resize(Buffers::Skeleton->Size() + skeleton.Bones.size());
            return { skeleton, skeletonInfo };
        }

    }

}
#pragma once

#include <anim/skeleton.h>

namespace xpe {

    namespace anim {

        using namespace ecs;

        class ENGINE_API mSkeletonManager final
        {
        public:
            static void Init();
            static void Free();

            static void Bind(u32 index);
            static void Unbind(u32 index);
            static void Flush(u32 index);

            template<typename... Args>
            static sSkeleton AddSkeleton(Args &&... args);

            static void RemoveSkeleton(u32 index);

            static render::BoneBuffer* GetBuffer(u32 index);

        private:
            static vector<render::BoneBuffer*>* s_Buffers;
        };

        template<typename... Args>
        sSkeleton mSkeletonManager::AddSkeleton(Args &&... args)
        {
            sSkeleton skeleton(std::forward<Args>(args)...);
            skeleton.Index = s_Buffers->size();
            s_Buffers->emplace_back(new render::BoneBuffer());
            return skeleton;
        }

    }

}
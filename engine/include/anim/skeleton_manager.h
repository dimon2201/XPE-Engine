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

            static void Bind(u32 index);
            static void Unbind(u32 index);
            static void Flush(u32 index);

            template<typename... Args>
            static sSkeleton AddSkeleton(Args &&... args);

            static void RemoveSkeleton(u32 index);

            static render::sBoneBuffer* GetBuffer(u32 index);

        private:
            static vector<render::sBoneBuffer*>* s_Buffers;
        };

        template<typename... Args>
        sSkeleton cSkeletonManager::AddSkeleton(Args &&... args)
        {
            sSkeleton skeleton(std::forward<Args>(args)...);
            skeleton.Index = s_Buffers->size();
            s_Buffers->emplace_back(new render::sBoneBuffer());
            return skeleton;
        }

    }

}
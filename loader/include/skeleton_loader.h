#pragma once

#include <build.h>
#include <assimp_manager.h>

#include <anim/skeleton_manager.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace anim;
        using namespace math;

        class LOADER_API SkeletonLoader final
        {
        public:
            static Skeleton Load(const char* filepath, const vector<eLoadOption>& options = {
                    eLoadOption::TRIANGULATE
            });
        };

    }

}
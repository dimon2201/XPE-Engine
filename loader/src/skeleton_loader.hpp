#pragma once

#include <build.hpp>
#include <assimp_manager.hpp>

#include <anim/skeleton_manager.hpp>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace anim;
        using namespace math;

        class LOADER_API cSkeletonLoader final
        {
        public:
            static std::pair<sSkeleton, sSkeletonInfo> Load(const char* filepath, const vector<eLoadOption>& options = {
                    eLoadOption::TRIANGULATE
            });
        };

    }

}
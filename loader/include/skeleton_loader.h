#pragma once

#include <build.h>
#include <assimp_types.h>

#include <anim/skeleton.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace anim;
        using namespace math;

        class LOADER_API SkeletonLoader : public Object
        {

        public:
            Ref<Skeleton> Load(const char* filepath, const vector<eLoadOption>& options = {
                    eLoadOption::TRIANGULATE
            });

        private:
            unordered_map<string, Ref<Skeleton>> m_Map;

        };

    }

}
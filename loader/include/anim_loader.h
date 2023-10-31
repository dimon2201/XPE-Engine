#pragma once

#include <build.h>
#include <assimp_manager.h>

#include <anim/anim.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace anim;
        using namespace math;
        using namespace ecs;

        class LOADER_API AnimLoader final
        {
        public:
            static sAnimation Load(const char* filepath, const vector<eLoadOption>& options = {
                    eLoadOption::TRIANGULATE
            });
        };

    }

}
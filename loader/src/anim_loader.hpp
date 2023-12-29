#pragma once

#include <build.hpp>
#include <assimp_manager.hpp>

#include <anim/anim.hpp>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace anim;
        using namespace math;
        using namespace ecs;

        class LOADER_API cAnimLoader final
        {
        public:
            static sAnimation Load(const char* filepath, const vector<eLoadOption>& options = {
                    eLoadOption::TRIANGULATE
            });
        };

    }

}
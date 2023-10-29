#pragma once

#include <build.h>
#include <assimp_manager.h>

#include <rendering/geometry/geometry_manager.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace render;
        using namespace math;

        class LOADER_API ModelLoader final
        {
        public:
            static Model Load(const char* filepath, const vector<eLoadOption>& options = {
                    eLoadOption::TRIANGULATE,
                    eLoadOption::FLIP_UV,
                    eLoadOption::CALC_TANGENTS,
                    eLoadOption::OPTIMIZE_MESHES,
                    eLoadOption::GEN_NORMAL,
                    eLoadOption::JOIN_VERTICES
            });
        };

    }

}
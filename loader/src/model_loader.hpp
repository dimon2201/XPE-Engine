#pragma once

#include <build.hpp>
#include <assimp_manager.hpp>

#include <rendering/geometry/geometry_manager.hpp>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace render;
        using namespace math;

        class LOADER_API cModelLoader final
        {
        public:
            static sModel Load(const char* filepath, const vector<eLoadOption>& options = {
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
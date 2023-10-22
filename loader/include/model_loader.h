#pragma once

#include <build.h>
#include <assimp_types.h>

#include <geometry/geometry_manager.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace render;
        using namespace math;

        class LOADER_API ModelLoader : public Object
        {

        public:
            Ref<Model> Load(const char* filepath, const vector<eLoadOption>& options = {
                    eLoadOption::TRIANGULATE,
                    eLoadOption::FLIP_UV,
                    eLoadOption::CALC_TANGENTS,
                    eLoadOption::OPTIMIZE_MESHES,
                    eLoadOption::GEN_NORMAL,
                    eLoadOption::JOIN_VERTICES
            });

        private:
            unordered_map<string, Ref<Model>> m_Map;

        };

    }

}
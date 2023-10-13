#pragma once

#include <build.h>
#include <assimp_types.h>

#include <geometry/geometry_manager.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace anim;
        using namespace math;

        class LOADER_API SkinLoader : public Object
        {

        public:
            Ref<SkinModel> Load(const char* filepath, const vector<eLoadOption>& options = {
                    eLoadOption::TRIANGULATE,
                    eLoadOption::CALC_TANGENTS,
                    eLoadOption::FLIP_UV,
                    eLoadOption::OPTIMIZE_MESHES,
                    eLoadOption::GEN_NORMAL,
                    eLoadOption::JOIN_VERTICES
            });

        private:
            unordered_map<string, Ref<SkinModel>> m_Map;
        };

    }

}
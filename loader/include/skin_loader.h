#pragma once

#include <build.h>
#include <assimp_types.h>

#include <anim/storages/skin_storage.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace anim;
        using namespace math;

        class LOADER_API SkinLoader : public Object
        {

        public:
            SkinLoader(SkinStorage* skinStorage) : m_Storage(skinStorage) {}

            Ref<SkinModel> Load(const char* filepath, const vector<eLoadOption>& options = {
                    eLoadOption::TRIANGULATE,
                    eLoadOption::CALC_TANGENTS,
                    eLoadOption::FLIP_UV,
                    eLoadOption::OPTIMIZE_MESHES,
                    eLoadOption::GEN_NORMAL,
                    eLoadOption::JOIN_VERTICES
            });

        private:
            SkinStorage* m_Storage;
        };

    }

}
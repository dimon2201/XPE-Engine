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
            enum class eOption
            {
                TRIANGULATE,
                FLIP_UV,
                CALC_TANGENTS,
                OPTIMIZE_MESHES
            };

            SkinLoader(SkinStorage* skinStorage) : m_Storage(skinStorage) {}

            Ref<SkinModel> Load(const char* filepath, const vector<eOption>& options = {
                    eOption::TRIANGULATE,
                    eOption::CALC_TANGENTS,
                    eOption::OPTIMIZE_MESHES,
            });

        private:
            SkinStorage* m_Storage;
        };

    }

}
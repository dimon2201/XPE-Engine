#pragma once

#include <build.h>
#include <assimp_types.h>

#include <anim/storages/anim_storage.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace anim;
        using namespace math;

        class LOADER_API AnimLoader : public Object
        {

        public:
            enum class eOption
            {
                TRIANGULATE,
                FLIP_UV,
                CALC_TANGENTS,
                OPTIMIZE_MESHES
            };

            AnimLoader(AnimStorage* animStorage) : m_Storage(animStorage) {}

            Ref<Animation> Load(const char* filepath, const vector<eOption>& options = {
                    eOption::TRIANGULATE
            });

        private:
            AnimStorage* m_Storage;
        };

    }

}
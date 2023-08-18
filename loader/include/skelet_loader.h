#pragma once

#include <build.h>
#include <assimp_types.h>

#include <anim/storages/skelet_storage.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace anim;
        using namespace math;

        class LOADER_API SkeletLoader : public Object
        {

        public:
            enum class eOption
            {
                TRIANGULATE,
                FLIP_UV,
                CALC_TANGENTS,
                OPTIMIZE_MESHES
            };

            SkeletLoader(SkeletStorage* skeletStorage) : m_Storage(skeletStorage) {}

            Ref<Skelet> Load(const char* filepath, const vector<eOption>& options = {
                    eOption::TRIANGULATE
            });

        private:
            SkeletStorage* m_Storage;
        };

    }

}
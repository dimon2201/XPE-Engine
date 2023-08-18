#pragma once

#include <build.h>
#include <assimp_types.h>

#include <rendering/storages/geometry_storage.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace render;
        using namespace math;

        class LOADER_API ModelLoader : public Object
        {

        public:
            enum class eOption
            {
                TRIANGULATE,
                FLIP_UV,
                CALC_TANGENTS,
                OPTIMIZE_MESHES
            };

            ModelLoader(GeometryStorage* geometryStorage) : m_Storage(geometryStorage) {}

            Ref<Model3D> Load(const char* filepath, const vector<eOption>& options = {
                    eOption::TRIANGULATE,
                    eOption::CALC_TANGENTS,
                    eOption::OPTIMIZE_MESHES,
            });

        private:
            GeometryStorage* m_Storage;
        };

    }

}
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
            ModelLoader(GeometryStorage* geometryStorage) : m_Storage(geometryStorage) {}

            Ref<Model3D> Load(const char* filepath, const vector<eLoadOption>& options = {
                    eLoadOption::TRIANGULATE,
                    eLoadOption::FLIP_UV,
                    eLoadOption::CALC_TANGENTS,
                    eLoadOption::OPTIMIZE_MESHES,
                    eLoadOption::GEN_NORMAL,
                    eLoadOption::JOIN_VERTICES
            });

        private:
            GeometryStorage* m_Storage;
        };

    }

}
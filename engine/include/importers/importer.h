#pragma once

#include <geometry/mesh.h>
#include <importers/gltf/gltf.hpp>

namespace xpe {

    namespace io {

        class ENGINE_API Importer final {

        public:
            static bool Import(const char* filepath, math::Model3D& model);

        };

    }

}

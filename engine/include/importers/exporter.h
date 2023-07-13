#pragma once

#include <geometry/mesh.h>

namespace xpe {

    namespace io {

        class ENGINE_API Exporter final {

        public:
            static bool Export(const char* filepath, const math::Model3D& model);

        private:
            static void FromGLTFModel(const math::Model3D& model, void* data);

        };

    }

}
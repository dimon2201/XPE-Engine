#pragma once

#include <rendering/geometry/geometries.h>

namespace xpe {

    namespace render {

        class ENGINE_API cGeometryManager final
        {
        public:
            static void Init();
            static void Free();
            static std::pair<sGeometry, sGeometryInfo> AddGeometry(const sGeometry& _geometry);
        };

    }

}
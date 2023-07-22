#pragma once

#include <geometry/geometry.h>

namespace xpe {

    namespace math {

        struct ENGINE_API SphereGeometry : GeometryIndexed<render::Vertex3D> {
            int XSegments = 64;
            int YSegments = 64;

            SphereGeometry();
            SphereGeometry(int xSegments, int ySegments);
            ~SphereGeometry();

        private:
            void Init();
        };

    }

}
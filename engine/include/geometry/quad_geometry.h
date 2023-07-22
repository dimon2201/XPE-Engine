#pragma once

#include <geometry/geometry.h>

namespace xpe {

    namespace math {

        struct ENGINE_API Quad2d : GeometryIndexed<render::Vertex2D> {
            Quad2d();
            ~Quad2d();
        };

        struct ENGINE_API Quad : GeometryIndexed<render::Vertex3D> {
            Quad();
            ~Quad();
        };

    }

}
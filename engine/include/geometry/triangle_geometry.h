#pragma once

#include <geometry/geometry.h>

namespace xpe {

    namespace math {

        struct ENGINE_API Triangle2d : GeometryVertexed<render::Vertex2D> {
            Triangle2d();
            ~Triangle2d();
        };

        struct ENGINE_API Triangle3d : GeometryVertexed<render::Vertex3D> {
            Triangle3d();
            ~Triangle3d();
        };

    }

}
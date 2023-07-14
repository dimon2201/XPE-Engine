#pragma once

#include <geometry/geometry.h>

namespace xpe {

    namespace math {

        struct ENGINE_API Quad2d : GeometryVertexed<render::Vertex2D> {
            Quad2d();
            ~Quad2d();
        };

        struct ENGINE_API Quad3d : GeometryVertexed<render::Vertex3D> {
            Quad3d();
            ~Quad3d();
        };

    }

}
#pragma once

#include <geometry/geometry.h>

namespace xpe {

    namespace math {

        struct ENGINE_API Quad2d : GeometryVertexed<render::Vertex2D> {
            Quad2d();
            ~Quad2d();
        };

        struct ENGINE_API Quad : GeometryVertexed<render::Vertex3D> {
            Quad();
            ~Quad();
        };

    }

}
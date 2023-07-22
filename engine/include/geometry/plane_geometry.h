#pragma once

#include <geometry/geometry.h>

namespace xpe {

    namespace math {

        struct ENGINE_API PlaneGeometry : GeometryIndexed<render::Vertex3D> {
            int Size = 16;

            PlaneGeometry();
            PlaneGeometry(int size);
            ~PlaneGeometry();

        private:
            void Init();
        };

    }

}
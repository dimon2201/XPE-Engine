#pragma once

#include <geometry/geometry.h>

namespace xpe {

    namespace math {

        struct ENGINE_API PlaneGeometry : Geometry<render::Vertex3D> {
            int Size = 64;

            PlaneGeometry();
            PlaneGeometry(int size);
            ~PlaneGeometry();

        private:
            void Init();
        };

    }

}
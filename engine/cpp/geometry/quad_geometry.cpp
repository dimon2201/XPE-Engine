#include <geometry/quad_geometry.h>

namespace xpe {

    namespace math {

        Quad2d::Quad2d() {
            Vertices[0].Position = { -0.5f, -0.5f };
            Vertices[1].Position = { -0.5f, 0.5f };
            Vertices[2].Position = { 0.5f, 0.5f };
            Vertices[3].Position = { 0.5f, -0.5f };

            InitUV(&Vertices[0], &Vertices[1], &Vertices[2], &Vertices[3]);
        }

        Quad2d::~Quad2d() {
            Vertices.Free();
        }

        math::Quad3d::Quad3d() {
            Vertices[0].Position = { -0.5f, -0.5f, 0 };
            Vertices[1].Position = { -0.5f, 0.5f, 0 };
            Vertices[2].Position = { 0.5f, 0.5f, 0 };
            Vertices[3].Position = { 0.5f, -0.5f, 0 };

            InitUV(&Vertices[0], &Vertices[1], &Vertices[2], &Vertices[3]);
            InitNormal(&Vertices[0], &Vertices[1], &Vertices[2], &Vertices[3]);
        }

        math::Quad3d::~Quad3d() {
            Vertices.Free();
        }

    }

}
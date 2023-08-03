#include <geometry/triangle_geometry.h>

namespace xpe {

    namespace math {

        Triangle2d::Triangle2d()
        {
            Vertices.Init(3);

            auto& v0 = Vertices[0];
            auto& v1 = Vertices[1];
            auto& v2 = Vertices[2];

            v0.Position = { 0, 0.5f };
            v1.Position = { 0.45f, -0.5f };
            v2.Position = { -0.45f, -0.5f };

            v0.UV = { 0, 1 };
            v1.UV = { 1, 0 };
            v2.UV = { 1, 1 };

            PrimitiveTopology = render::ePrimitiveTopology::TRIANGLE_STRIP;
        }

        Triangle2d::~Triangle2d()
        {
            Vertices.Free();
        }

        Triangle::Triangle()
        {
            Vertices.Init(3);

            auto& v0 = Vertices[0];
            auto& v1 = Vertices[1];
            auto& v2 = Vertices[2];

            v0.Position = { -0.5f, -0.5f, 0 };
            v1.Position = { 0.5f, -0.5f, 0 };
            v2.Position = { 0, 0.5f, 0 };

            v0.UV = { 0, 1 };
            v1.UV = { 1, 0 };
            v2.UV = { 1, 1 };

            InitNormal<render::Vertex3D>(&v0, &v1, &v2);

            PrimitiveTopology = render::ePrimitiveTopology::TRIANGLE_STRIP;
        }

        Triangle::~Triangle()
        {
            Vertices.Free();
        }

    }

}
#include <geometry/quad_geometry.h>

namespace xpe {

    namespace math {

        Quad2d::Quad2d()
        {
            Vertices.Init(4);
            Indices.Init(6);

            Vertices[0].Position = { -0.5f, -0.5f };
            Vertices[1].Position = { -0.5f, 0.5f };
            Vertices[2].Position = { 0.5f, 0.5f };
            Vertices[3].Position = { 0.5f, -0.5f };

            InitUV(&Vertices[0], &Vertices[1], &Vertices[2], &Vertices[3]);

            Indices.Data = { 0, 1, 2, 2, 3, 0 };

            PrimitiveTopology = render::ePrimitiveTopology::TRIANGLE_LIST;
        }

        Quad2d::~Quad2d()
        {
            Vertices.Free();
            Indices.Free();
        }

        Quad::Quad()
        {
            Vertices.Init(4);
            Indices.Init(6);

            Vertices[0].Position = { -0.5f, -0.5f, 0 };
            Vertices[1].Position = { -0.5f, 0.5f, 0 };
            Vertices[2].Position = { 0.5f, 0.5f, 0 };
            Vertices[3].Position = { 0.5f, -0.5f, 0 };

            InitUV(&Vertices[0], &Vertices[1], &Vertices[2], &Vertices[3]);
            InitNormal(&Vertices[0], &Vertices[1], &Vertices[2], &Vertices[3]);

            Indices.Data = { 0, 1, 2, 2, 3, 0 };

            PrimitiveTopology = render::ePrimitiveTopology::TRIANGLE_LIST;
        }

        Quad::~Quad()
        {
            Vertices.Free();
            Indices.Free();
        }

    }

}
#include <geometry/cube_geometry.h>

namespace xpe {

    namespace math {

        CubeGeometry::CubeGeometry()
        {
            Vertices.Init(24);

            Vertices[0] = { { -0.5f,0.5f,-0.5f } };
            Vertices[1] = { { -0.5f,-0.5f,-0.5f } };
            Vertices[2] = { { 0.5f,-0.5f,-0.5f } };
            Vertices[3] = { { 0.5f,0.5f,-0.5f } };

            Vertices[4] = { { -0.5f, 0.5f, 0.5f } };
            Vertices[5] = { { -0.5f,-0.5f,0.5f } };
            Vertices[6] = { { 0.5f,-0.5f,0.5f } };
            Vertices[7] = { { 0.5f,0.5f,0.5f } };

            Vertices[8] = { { 0.5f,0.5f,-0.5f } };
            Vertices[9] = { { 0.5f,-0.5f,-0.5f } };
            Vertices[10] = { { 0.5f,-0.5f,0.5f } };
            Vertices[11] = { { 0.5f,0.5f,0.5f } };

            Vertices[12] = { { -0.5f,0.5f,-0.5f } };
            Vertices[13] = { { -0.5f,-0.5f,-0.5f } };
            Vertices[14] = { { -0.5f,-0.5f,0.5f } };
            Vertices[15] = { { -0.5f,0.5f,0.5f } };

            Vertices[16] = { { -0.5f,0.5f,0.5f } };
            Vertices[17] = { { -0.5f,0.5f,-0.5f } };
            Vertices[18] = { { 0.5f,0.5f,-0.5f } };
            Vertices[19] = { { 0.5f,0.5f,0.5f } };

            Vertices[20] = { { -0.5f,-0.5f,0.5f } };
            Vertices[21] = { { -0.5f,-0.5f,-0.5f } };
            Vertices[22] = { { 0.5f,-0.5f,-0.5f } };
            Vertices[23] = { { 0.5f,-0.5f,0.5f } };

            InitUV<render::Vertex3D>(*this);
            InitNormal<render::Vertex3D>(*this);

            Indices.Init(36);
            u32 indices[36] = {
                0,1,3,
                3,1,2,
                4,5,7,
                7,5,6,
                8,9,11,
                11,9,10,
                12,13,15,
                15,13,14,
                16,17,19,
                19,17,18,
                20,21,23,
                23,21,22
            };
            Indices.MoveFrom(indices, 36);

            InitTBN<render::Vertex3D>(*this);

            PrimitiveTopology = render::ePrimitiveTopology::TRIANGLE_LIST;
        }

        CubeGeometry::~CubeGeometry()
        {
            Vertices.Free();
            Indices.Free();
        }

    }

}
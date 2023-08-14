#include <geometry/geometries.h>

namespace xpe {

    namespace math {

        GeometryVertexed<Vertex2D> Triangle2D()
        {
            GeometryVertexed<Vertex2D> geometry;
            auto& vertices = geometry.Vertices;

            vertices.Init(3);

            vertices[0].Position = { -0.5f, -0.5f };
            vertices[1].Position = { 0.5f, -0.5f };
            vertices[2].Position = { 0, 0.5f };

            vertices[0].UV = { 0, 1 };
            vertices[1].UV = { 1, 0 };
            vertices[2].UV = { 1, 1 };

            geometry.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_STRIP;

            return geometry;
        }

        GeometryVertexed<Vertex3D> Triangle()
        {
            GeometryVertexed<Vertex3D> geometry;
            auto& vertices = geometry.Vertices;

            vertices.Init(3);

            vertices[0].Position = { -0.5f, -0.5f, 0 };
            vertices[1].Position = { 0.5f, -0.5f, 0 };
            vertices[2].Position = { 0, 0.5f, 0 };

            vertices[0].UV = { 0, 1 };
            vertices[1].UV = { 1, 0 };
            vertices[2].UV = { 1, 1 };

            InitNormal<Vertex3D>(&vertices[0], &vertices[1], &vertices[2]);

            geometry.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_STRIP;

            return geometry;
        }

        GeometryIndexed<Vertex2D> Quad2D()
        {
            GeometryIndexed<Vertex2D> geometry;
            auto& vertices = geometry.Vertices;
            auto& indices = geometry.Indices;

            vertices.Init(4);
            indices.Init(6);

            vertices[0].Position = { -0.5f, -0.5f };
            vertices[1].Position = { -0.5f, 0.5f };
            vertices[2].Position = { 0.5f, 0.5f };
            vertices[3].Position = { 0.5f, -0.5f };

            InitUV(&vertices[0], &vertices[1], &vertices[2], &vertices[3]);

            indices.Data = { 0, 1, 2, 2, 3, 0 };

            geometry.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_LIST;

            return geometry;
        }

        GeometryIndexed<Vertex3D> Quad()
        {
            GeometryIndexed<Vertex3D> geometry;
            auto& vertices = geometry.Vertices;
            auto& indices = geometry.Indices;

            vertices.Init(4);
            indices.Init(6);

            vertices[0].Position = { -0.5f, -0.5f, 0 };
            vertices[1].Position = { -0.5f, 0.5f, 0 };
            vertices[2].Position = { 0.5f, 0.5f, 0 };
            vertices[3].Position = { 0.5f, -0.5f, 0 };

            InitUV(&vertices[0], &vertices[1], &vertices[2], &vertices[3]);
            InitNormal(&vertices[0], &vertices[1], &vertices[2], &vertices[3]);

            indices.Data = { 0, 1, 2, 2, 3, 0 };

            geometry.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_LIST;

            return geometry;
        }

        GeometryIndexed<Vertex3D> Cube()
        {
            GeometryIndexed<Vertex3D> geometry;
            auto& vertices = geometry.Vertices;
            auto& indices = geometry.Indices;

            vertices.Init(24);
            indices.Init(36);

            vertices[0] = { { -0.5f,0.5f,-0.5f } };
            vertices[1] = { { -0.5f,-0.5f,-0.5f } };
            vertices[2] = { { 0.5f,-0.5f,-0.5f } };
            vertices[3] = { { 0.5f,0.5f,-0.5f } };

            vertices[4] = { { -0.5f, 0.5f, 0.5f } };
            vertices[5] = { { -0.5f,-0.5f,0.5f } };
            vertices[6] = { { 0.5f,-0.5f,0.5f } };
            vertices[7] = { { 0.5f,0.5f,0.5f } };

            vertices[8] = { { 0.5f,0.5f,-0.5f } };
            vertices[9] = { { 0.5f,-0.5f,-0.5f } };
            vertices[10] = { { 0.5f,-0.5f,0.5f } };
            vertices[11] = { { 0.5f,0.5f,0.5f } };

            vertices[12] = { { -0.5f,0.5f,-0.5f } };
            vertices[13] = { { -0.5f,-0.5f,-0.5f } };
            vertices[14] = { { -0.5f,-0.5f,0.5f } };
            vertices[15] = { { -0.5f,0.5f,0.5f } };

            vertices[16] = { { -0.5f,0.5f,0.5f } };
            vertices[17] = { { -0.5f,0.5f,-0.5f } };
            vertices[18] = { { 0.5f,0.5f,-0.5f } };
            vertices[19] = { { 0.5f,0.5f,0.5f } };

            vertices[20] = { { -0.5f,-0.5f,0.5f } };
            vertices[21] = { { -0.5f,-0.5f,-0.5f } };
            vertices[22] = { { 0.5f,-0.5f,-0.5f } };
            vertices[23] = { { 0.5f,-0.5f,0.5f } };

            InitUV<Vertex3D>(vertices);
            InitNormal<Vertex3D>(vertices);
            InitTBN<Vertex3D>(vertices);

            indices.Data = {
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

            geometry.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_LIST;

            return geometry;
        }

        GeometryIndexed<Vertex3D> Plane(s32 size)
        {
            GeometryIndexed<Vertex3D> geometry;

            geometry.Vertices.Init(size * size);

            float shiftX = -size / 2.0f;
            float shiftZ = shiftX;
            int k = 0;

            // TODO: we can probably speed up this generation time
            for (int i = 0 ; i < size ; i++) {
                for (int j = 0 ; j < size ; j++) {
                    auto& v = geometry.Vertices[k];
                    v.Position = { shiftX + i, 0, shiftZ + j };
                    v.UV = { (float) i / (float) size, (float) j / (float) size };

                    if (k % 3 == 0 && k > 0) {
                        InitNormal(
                            &geometry.Vertices[k - 3],
                            &geometry.Vertices[k - 2],
                            &geometry.Vertices[k - 1],
                            &geometry.Vertices[k]
                        );
                    }

                    k++;
                }
            }

            geometry.Indices.Init((size - 1) * (size - 1) * 6);
            k = 0;

            // TODO: we can probably speed up this generation time
            for (int i = 0; i < size - 1; i++) {
                for (int j = 0; j < size - 1; j++) {
                    u32 bottomLeft = i * size + j;
                    u32 bottomRight = i * size + j + 1;
                    u32 topLeft = (i + 1) * size + j;
                    u32 topRight = (i + 1) * size + j + 1;
                    // top-left triangle
                    geometry.Indices[k++] = bottomLeft;
                    geometry.Indices[k++] = topLeft;
                    geometry.Indices[k++] = topRight;
                    // bottom-right triangle
                    geometry.Indices[k++] = bottomLeft;
                    geometry.Indices[k++] = topRight;
                    geometry.Indices[k++] = bottomRight;
                }
            }

            geometry.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_STRIP;

            return geometry;
        }

        GeometryIndexed<Vertex3D> Sphere(s32 xSegments, s32 ySegments)
        {
            GeometryIndexed<Vertex3D> geometry;
            geometry.Vertices.Init((xSegments + 1) * (ySegments + 1));

            int i = 0;
            for (u32 x = 0; x <= xSegments; x++)
            {
                for (u32 y = 0; y <= ySegments; y++)
                {
                    auto& V = geometry.Vertices[i];

                    float xSegment = (float)x / (float)xSegments;
                    float ySegment = (float)y / (float)ySegments;

                    float xPos = cos(xSegment * 2.0f * PI) * sin(ySegment * PI);
                    float yPos = cos(ySegment * PI);
                    float zPos = sin(xSegment * 2.0f * PI) * sin(ySegment * PI);

                    V.Position = { xPos, yPos, zPos };
                    V.UV = { xSegment, ySegment };
                    V.Normal = glm::normalize(V.Position);

                    // calculate tangent
                    // todo: it will not always give a correct tangents.
                    // todo: we need to check for different case of  [(theta,phi+pi/2);(theta+pi/2,phi);(theta+pi/2,phi+pi/2)]
                    glm::vec3 tangent;
                    float r = V.Position.length();
                    float theta = acos(zPos / r);
                    float phi = atan2(yPos, xPos) + PI * 0.5;
                    tangent.x = sin(theta) * cos(phi);
                    tangent.y = sin(theta) * sin(phi);
                    tangent.z = cos(theta);
                    V.Tangent = tangent;

                    i++;
                }
            }

            bool oddRow = false;
            vector<u32> indexList;
            indexList.resize(xSegments * ySegments + 1);

            for (u32 y = 0; y < ySegments; y++)
            {
                if (!oddRow) {
                    for (u32 x = 0; x <= xSegments; x++)
                    {
                        indexList.push_back(y * (xSegments + 1) + x);
                        indexList.push_back((y + 1) * (xSegments + 1) + x);
                    }
                }
                else {
                    for (int x = xSegments; x >= 0; x--)
                    {
                        indexList.push_back((y + 1) * (xSegments + 1) + x);
                        indexList.push_back(y * (xSegments + 1) + x);
                    }
                }
                oddRow = !oddRow;
            }

            geometry.Indices.Init(indexList.size());
            geometry.Indices.Data = indexList;

            geometry.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_STRIP;

            return geometry;
        }

        Mesh::Mesh(usize vertexCount, usize indexCount)
        {
            Vertices.Init(vertexCount);
            Indices.Init(indexCount);
        }

    }

}
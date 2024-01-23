#include <rendering/geometry/geometries.hpp>

namespace xpe {

    namespace render {

        sTriangle::sTriangle()
        {
            PrimitiveTopology = ePrimitiveTopology::TRIANGLE_STRIP;

            auto& vertices = Vertices;

            vertices.resize(3);

            vertices[0].Position = { -0.5f, -0.5f, 0 };
            vertices[1].Position = { 0.5f, -0.5f, 0 };
            vertices[2].Position = { 0, 0.5f, 0 };

            vertices[0].UV = { 0, 1 };
            vertices[1].UV = { 1, 0 };
            vertices[2].UV = { 1, 1 };

            InitNormal(&vertices[0], &vertices[1], &vertices[2]);
        }

        sLine::sLine()
        {
            PrimitiveTopology = ePrimitiveTopology::LINE_LIST;
        }

        sQuad::sQuad()
        {
            PrimitiveTopology = ePrimitiveTopology::TRIANGLE_LIST;

            auto& vertices = Vertices;
            auto& indices = Indices;

            vertices.resize(4);
            indices.resize(6);

            vertices[0].Position = { -0.5f, -0.5f, 0 };
            vertices[1].Position = { -0.5f, 0.5f, 0 };
            vertices[2].Position = { 0.5f, 0.5f, 0 };
            vertices[3].Position = { 0.5f, -0.5f, 0 };

            InitUV(&vertices[0], &vertices[1], &vertices[2], &vertices[3]);
            InitNormal(&vertices[0], &vertices[1], &vertices[2], &vertices[3]);

            indices = { 0, 1, 2, 2, 3, 0 };
        }

        sCube::sCube()
        {
            PrimitiveTopology = ePrimitiveTopology::TRIANGLE_LIST;

            auto& vertices = Vertices;
            auto& indices = Indices;

            vertices.resize(24);
            indices.resize(36);

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

            InitUV(vertices);
            InitNormal(vertices);
            InitTangent(vertices);

            indices = {
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
        }

        sPlane::sPlane(s32 size)
        {
            PrimitiveTopology = ePrimitiveTopology::TRIANGLE_STRIP;

            auto& vertices = Vertices;
            vertices.resize(size * size);

            float shiftX = -size / 2.0f;
            float shiftZ = shiftX;
            int k = 0;

            // TODO: we can probably speed up this generation time
            for (int i = 0 ; i < size ; i++) {
                for (int j = 0 ; j < size ; j++) {
                    auto& v = vertices[k];
                    v.Position = { shiftX + i, 0, shiftZ + j };
                    v.UV = { (float) i / (float) size, (float) j / (float) size };
                    v.Normal = { 0, 1, 0 };
                    v.Tangent = { 0, 0, 1 };
                    k++;
                }
            }

            auto& indices = Indices;
            indices.resize((size - 1) * (size - 1) * 6);
            k = 0;

            // TODO: we can probably speed up this generation time
            for (int i = 0; i < size - 1; i++) {
                for (int j = 0; j < size - 1; j++) {
                    u32 bottomLeft = i * size + j;
                    u32 bottomRight = i * size + j + 1;
                    u32 topLeft = (i + 1) * size + j;
                    u32 topRight = (i + 1) * size + j + 1;
                    // top-left triangle
                    indices[k++] = bottomLeft;
                    indices[k++] = topLeft;
                    indices[k++] = topRight;
                    // bottom-right triangle
                    indices[k++] = bottomLeft;
                    indices[k++] = topRight;
                    indices[k++] = bottomRight;
                }
            }
        }

        sSphere::sSphere(s32 xSegments, s32 ySegments)
        {
            PrimitiveTopology = ePrimitiveTopology::TRIANGLE_STRIP;

            auto& vertices = Vertices;
            vertices.resize((xSegments + 1) * (ySegments + 1));

            int i = 0;
            for (u32 x = 0; x <= xSegments; x++)
            {
                for (u32 y = 0; y <= ySegments; y++)
                {
                    auto& V = vertices[i];

                    float xSegment = (float)x / (float)xSegments;
                    float ySegment = (float)y / (float)ySegments;

                    float xPos = cos(xSegment * 2.0f * K_PI) * sin(ySegment * K_PI);
                    float yPos = cos(ySegment * K_PI);
                    float zPos = sin(xSegment * 2.0f * K_PI) * sin(ySegment * K_PI);

                    V.Position = { xPos, yPos, zPos };
                    V.UV = { xSegment, ySegment };
                    V.Normal = glm::normalize(V.Position);

                    // calculate tangent
                    // todo: it will not always give a correct tangents.
                    // todo: we need to check for different case of  [(theta,phi+pi/2);(theta+pi/2,phi);(theta+pi/2,phi+pi/2)]
                    glm::vec3 tangent;
                    float r = V.Position.length();
                    float theta = acos(zPos / r);
                    float phi = atan2(yPos, xPos) + K_PI * 0.5;
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

            Indices = indexList;
        }

        sCapsule::sCapsule(s32 segments)
        {
            PrimitiveTopology = ePrimitiveTopology::TRIANGLE_STRIP;

            auto& vertices = Vertices;
            vertices.resize(segments * 2);

            glm::vec3 origin = glm::vec3(0.0f);
            glm::vec3 top = origin + glm::vec3(0.0f, 0.5f, 0.0f);
            glm::vec3 bottom = origin - glm::vec3(0.0f, 0.5f, 0.0f);

            // Cylinder
            u32 pointer = 0;
            sVertex vertex;
            for (u32 i = 0; i < segments; i++)
            {
                f32 angle = (f32)i * (360.0f / (f32)segments);

                glm::vec3 topVertexPosition = glm::vec3(
                    origin.x + glm::cos(glm::radians(angle)),
                    top.y,
                    origin.z + glm::sin(glm::radians(angle))
                );

                glm::vec3 bottomVertexPosition = glm::vec3(
                    origin.x + glm::cos(glm::radians(angle)),
                    bottom.y,
                    origin.z + glm::sin(glm::radians(angle))
                );

                topVertexPosition.x *= 0.5f;
                topVertexPosition.z *= 0.5f;
                bottomVertexPosition.x *= 0.5f;
                bottomVertexPosition.z *= 0.5f;

                vertex.Position = bottomVertexPosition;
                vertex.Normal = glm::normalize(bottomVertexPosition - bottom);
                vertices[pointer++] = vertex;
                vertex.Position = topVertexPosition;
                vertex.Normal = glm::normalize(topVertexPosition - top);
                vertices[pointer++] = vertex;
            }

            usize cylinderVertexCount = pointer;

            // Top sphere
            sSphere topSphere;
            for (u32 i = 0; i < topSphere.Vertices.size(); i++)
            {
                topSphere.Vertices[i].Position *= 0.5f;
                topSphere.Vertices[i].Position.y += top.y;
                vertices.push_back(topSphere.Vertices[i]);
            }

            usize topSphereVertexCount = topSphere.Vertices.size();

            // Bottom sphere
            sSphere bottomSphere;
            for (u32 i = 0; i < bottomSphere.Vertices.size(); i++)
            {
                bottomSphere.Vertices[i].Position *= 0.5f;
                bottomSphere.Vertices[i].Position.y += bottom.y;
                vertices.push_back(bottomSphere.Vertices[i]);
            }

            vector<u32> indexList;
            indexList.resize((segments * 2) + 2);

            // Cylinder
            pointer = 0;
            for (u32 i = 0; i < segments; i++)
            {
                indexList[pointer++] = i * 2;
                indexList[pointer++] = (i * 2) + 1;
            }

            indexList[pointer++] = 0;
            indexList[pointer++] = 1;

            // Top sphere
            for (u32 i = 0; i < topSphere.Indices.size(); i++) {
                indexList.push_back(cylinderVertexCount + topSphere.Indices[i]);
            }

            // Bottom sphere
            for (u32 i = 0; i < bottomSphere.Indices.size(); i++) {
                indexList.push_back(cylinderVertexCount + topSphereVertexCount + bottomSphere.Indices[i]);
            }

            Indices = indexList;
        }

        sGeometry sModel::Merge()
        {
            sGeometry baked;
            baked.PrimitiveTopology = PrimitiveTopology;
            usize vertexCount = 0;
            for (auto& geometry : Geometries)
            {
                for (auto& vertex : geometry.Vertices)
                {
                    baked.Vertices.emplace_back(vertex);
                }
                for (auto& index : geometry.Indices)
                {
                    baked.Indices.emplace_back(index + vertexCount);
                }
                vertexCount += geometry.Vertices.size();
            }
            return baked;
        }

    }

}
#include <components.hpp>
#include <geometry_manager.hpp>

namespace xpe
{
    namespace ecs
    {
        const sMaterialFormat CMaterial::k_AlbedoFormat = { eTextureFormat::SRGBA8, 512, 512, K_SLOT_ALBEDO };
        const sMaterialFormat CMaterial::k_NormalFormat   = { eTextureFormat::RGBA8, 512, 512, K_SLOT_BUMPING };
        const sMaterialFormat CMaterial::k_ParallaxFormat = { eTextureFormat::R8, 512, 512, K_SLOT_PARALLAX };
        const sMaterialFormat CMaterial::k_MetalFormat = { eTextureFormat::R8, 512, 512, K_SLOT_METALLIC };
        const sMaterialFormat CMaterial::k_RoughnessFormat = { eTextureFormat::R8, 512, 512, K_SLOT_ROUGHNESS };
        const sMaterialFormat CMaterial::k_AoFormat = { eTextureFormat::R8, 512, 512, K_SLOT_AO };
        const sMaterialFormat CMaterial::k_EmissionFormat = { eTextureFormat::SRGBA8, 512, 512, K_SLOT_EMISSION };

        static void InitUV(sVertex* v0, sVertex* v1, sVertex* v2, sVertex* v3)
        {
            v0->UV = { 0, 0 };
            v1->UV = { 0, 1 };
            v2->UV = { 1, 1 };
            v3->UV = { 1, 0 };
        }

        static void InitNormal(sVertex* v0, sVertex* v1, sVertex* v2)
        {
            glm::vec3 x1 = v1->Position - v0->Position;
            glm::vec3 x2 = v2->Position - v0->Position;
            glm::vec3 normal = -glm::normalize(glm::cross(x1, x2));
            v0->Normal = normal;
            v1->Normal = normal;
            v2->Normal = normal;
        }

        static void InitNormal(sVertex* v0, sVertex* v1, sVertex* v2, sVertex* v3)
        {
            glm::vec3 x1 = v1->Position - v0->Position;
            glm::vec3 x2 = v3->Position - v0->Position;
            glm::vec3 normal = -glm::normalize(glm::cross(x1, x2));
            v0->Normal = normal;
            v1->Normal = normal;
            v2->Normal = normal;
            v3->Normal = normal;
        }

        static void InitTangent(sVertex* v0, sVertex* v1, sVertex* v2, sVertex* v3)
        {
            glm::vec3& pos0 = v0->Position;
            glm::vec3& pos1 = v1->Position;
            glm::vec3& pos2 = v2->Position;

            glm::vec2& uv0 = v0->UV;
            glm::vec2& uv1 = v1->UV;
            glm::vec2& uv2 = v2->UV;

            // Calculate sTriangle edges
            glm::vec3 dpos_1 = pos1 - pos0;
            glm::vec3 dpos_2 = pos2 - pos0;

            // Calculate delta UV
            glm::vec2 duv_1 = uv1 - uv0;
            glm::vec2 duv_2 = uv2 - uv0;

            // Calculate Tangents
            float r = 1.0f / (duv_1.x * duv_2.y - duv_1.y * duv_2.x);
            glm::vec3 tangent = (dpos_1 * duv_2.y - dpos_2 * duv_1.y) * r;
            v0->Tangent = tangent;
            v1->Tangent = tangent;
            v2->Tangent = tangent;
            v3->Tangent = tangent;
        }

        template<typename T>
        static void InitUV(vector<T>& vertices)
        {
            vertices[0].UV = { 0, 0 };
            vertices[1].UV = { 0, 1 };
            vertices[2].UV = { 1, 1 };
            vertices[3].UV = { 1, 0 };

            vertices[4].UV = { 0, 0 };
            vertices[5].UV = { 0, 1 };
            vertices[6].UV = { 1, 1 };
            vertices[7].UV = { 1, 0 };

            vertices[8].UV = { 0, 0 };
            vertices[9].UV = { 0, 1 };
            vertices[10].UV = { 1, 1 };
            vertices[11].UV = { 1, 0 };

            vertices[12].UV = { 0, 0 };
            vertices[13].UV = { 0, 1 };
            vertices[14].UV = { 1, 1 };
            vertices[15].UV = { 1, 0 };

            vertices[16].UV = { 0, 1 };
            vertices[17].UV = { 0, 0 };
            vertices[18].UV = { 1, 1 };
            vertices[19].UV = { 1, 0 };

            vertices[20].UV = { 0, 0 };
            vertices[21].UV = { 0, 1 };
            vertices[22].UV = { 1, 1 };
            vertices[23].UV = { 1, 0 };
        }

        template<typename T>
        static void InitNormal(vector<T>& vertices)
        {
            glm::vec3 normal1 = -glm::normalize(glm::cross(
                    vertices[1].Position - vertices[0].Position,
                    vertices[3].Position - vertices[0].Position
            ));

            glm::vec3 normal2 = glm::normalize(glm::cross(
                    vertices[5].Position - vertices[4].Position,
                    vertices[7].Position - vertices[4].Position
            ));

            glm::vec3 normal3 = -glm::normalize(glm::cross(
                    vertices[9].Position - vertices[8].Position,
                    vertices[11].Position - vertices[8].Position
            ));

            glm::vec3 normal4 = glm::normalize(glm::cross(
                    vertices[13].Position - vertices[12].Position,
                    vertices[15].Position - vertices[12].Position
            ));

            glm::vec3 normal5 = -glm::normalize(glm::cross(
                    vertices[17].Position - vertices[16].Position,
                    vertices[19].Position - vertices[16].Position
            ));

            glm::vec3 normal6 = glm::normalize(glm::cross(
                    vertices[21].Position - vertices[20].Position,
                    vertices[23].Position - vertices[20].Position
            ));

            vertices[0].Normal = normal1;
            vertices[1].Normal = normal1;
            vertices[2].Normal = normal1;
            vertices[3].Normal = normal1;

            vertices[4].Normal = normal2;
            vertices[5].Normal = normal2;
            vertices[6].Normal = normal2;
            vertices[7].Normal = normal2;

            vertices[8].Normal = normal3;
            vertices[9].Normal = normal3;
            vertices[10].Normal = normal3;
            vertices[11].Normal = normal3;

            vertices[12].Normal = normal4;
            vertices[13].Normal = normal4;
            vertices[14].Normal = normal4;
            vertices[15].Normal = normal4;

            vertices[16].Normal = normal5;
            vertices[17].Normal = normal5;
            vertices[18].Normal = normal5;
            vertices[19].Normal = normal5;

            vertices[20].Normal = normal6;
            vertices[21].Normal = normal6;
            vertices[22].Normal = normal6;
            vertices[23].Normal = normal6;
        }

        template<typename T>
        static void InitTangent(vector<T>& vertices)
        {
            InitTangent(&vertices[0], &vertices[1], &vertices[2], &vertices[3]);
            InitTangent(&vertices[4], &vertices[5], &vertices[6], &vertices[7]);
            InitTangent(&vertices[8], &vertices[9], &vertices[10], &vertices[11]);
            InitTangent(&vertices[12], &vertices[13], &vertices[14], &vertices[15]);
            InitTangent(&vertices[16], &vertices[17], &vertices[18], &vertices[19]);
            InitTangent(&vertices[20], &vertices[21], &vertices[22], &vertices[23]);
        }

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
    }
}
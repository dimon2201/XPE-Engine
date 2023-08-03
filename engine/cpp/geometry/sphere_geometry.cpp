#include <geometry/sphere_geometry.h>

namespace xpe {

    namespace math {

        SphereGeometry::SphereGeometry()
        {
            Init();
        }

        SphereGeometry::SphereGeometry(int xSegments, int ySegments)
        : XSegments(xSegments), YSegments(ySegments)
        {
            Init();
        }

        SphereGeometry::~SphereGeometry()
        {
            Vertices.Free();
            Indices.Free();
        }

        void SphereGeometry::Init()
        {
            int xSegments = XSegments;
            int ySegments = YSegments;

            Vertices.Init((xSegments + 1) * (ySegments + 1));

            int i = 0;
            for (u32 x = 0; x <= xSegments; x++) {
                for (u32 y = 0; y <= ySegments; y++) {
                    auto& V = Vertices[i];

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

            for (u32 y = 0; y < ySegments; y++) {
                if (!oddRow) {
                    for (u32 x = 0; x <= xSegments; x++) {
                        indexList.push_back(y * (xSegments + 1) + x);
                        indexList.push_back((y + 1) * (xSegments + 1) + x);
                    }
                }
                else {
                    for (int x = xSegments; x >= 0; x--) {
                        indexList.push_back((y + 1) * (xSegments + 1) + x);
                        indexList.push_back(y * (xSegments + 1) + x);
                    }
                }
                oddRow = !oddRow;
            }

            Indices.Init(indexList.size());
            Indices.MoveFrom(indexList.data(), indexList.size());

            PrimitiveTopology = render::ePrimitiveTopology::TRIANGLE_STRIP;
        }

    }

}
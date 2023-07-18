#include <geometry/sphere_geometry.h>

namespace xpe {

    namespace math {

        SphereGeometry::SphereGeometry() {
            Init();
        }

        SphereGeometry::SphereGeometry(int xSegments, int ySegments)
        : XSegments(xSegments), YSegments(ySegments) {
            Init();
        }

        SphereGeometry::~SphereGeometry() {
            Vertices.Free();
            Indices.Free();
        }

        void SphereGeometry::Init() {
            int xSegments = XSegments;
            int ySegments = YSegments;

            Vertices.Init((xSegments + 1) * (ySegments + 1));

            int i = 0;
            for (u32 x = 0; x <= xSegments; x++) {
                for (u32 y = 0; y <= ySegments; y++) {
                    auto& V = Vertices[i];

                    float xSegment = (float)x / (float)xSegments;
                    float ySegment = (float)y / (float)ySegments;

                    float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                    float yPos = std::cos(ySegment * PI);
                    float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                    V.Position = { xPos, yPos, zPos };
                    V.UV = { xSegment, ySegment };
                    V.Normal = { xPos, yPos, zPos };

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
            Indices.CopyFrom(indexList.data(), indexList.size());

            PrimitiveTopology = render::ePrimitiveTopology::TRIANGLE_STRIP;
        }

    }

}
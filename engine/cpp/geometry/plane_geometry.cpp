#include <geometry/plane_geometry.h>

namespace xpe {

    namespace math {

        PlaneGeometry::PlaneGeometry() {
            Init();
        }

        PlaneGeometry::PlaneGeometry(int size) : Size(size) {
            Init();
        }

        void PlaneGeometry::Init() {
            int size = Size;
            Vertices.Init(size * size);

            float shiftX = -size / 2.0f;
            float shiftZ = shiftX;
            int k = 0;

            // TODO: we can probably speed up this generation time
            for (int i = 0 ; i < size ; i++) {
                for (int j = 0 ; j < size ; j++) {
                    auto& v = Vertices[k];
                    v.Position = { shiftX + i, 0, shiftZ + j };
                    v.UV = { (float) i / (float) size, (float) j / (float) size };
                    k++;

                    if (k % 3 == 0) {
                        InitNormal(&Vertices[k - 3], &Vertices[k - 2], &Vertices[k - 1], &Vertices[k]);
                    }
                }
            }

            Indices.Init((size - 1) * (size - 1) * 6);
            k = 0;

            // TODO: we can probably speed up this generation time
            for (int i = 0; i < size - 1; i++) {
                for (int j = 0; j < size - 1; j++) {
                    u32 bottomLeft = i * size + j;
                    u32 bottomRight = i * size + j + 1;
                    u32 topLeft = (i + 1) * size + j;
                    u32 topRight = (i + 1) * size + j + 1;
                    // top-left triangle
                    Indices[k++] = bottomLeft;
                    Indices[k++] = topLeft;
                    Indices[k++] = topRight;
                    // bottom-right triangle
                    Indices[k++] = bottomLeft;
                    Indices[k++] = topRight;
                    Indices[k++] = bottomRight;
                }
            }
        }

        PlaneGeometry::~PlaneGeometry() {
            Vertices.Free();
            Indices.Free();
        }

    }

}
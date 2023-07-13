#include <geometry/mesh.h>

namespace xpe {

    namespace math {

        Mesh::Mesh(usize vertexCount, usize indexCount) {
            Vertices.Init(vertexCount);
            Indices.Init(indexCount);
        }

    }

}
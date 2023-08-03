#include <geometry/mesh_geometry.h>

namespace xpe {

    namespace math {

        Mesh::Mesh(usize vertexCount, usize indexCount)
        {
            Vertices.Init(vertexCount);
            Indices.Init(indexCount);
        }

        Mesh::~Mesh()
        {
            Vertices.Free();
            Indices.Free();
        }

    }

}
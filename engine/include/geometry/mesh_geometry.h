#pragma once

#include <geometry/geometry.h>
#include <rendering/materials/material.h>

namespace xpe {

    namespace math {

        struct ENGINE_API Mesh : GeometryIndexed<render::Vertex3D> {
            u32 MaterialIndex = 0;

            Mesh() = default;

            Mesh(usize vertexCount, usize indexCount);

            ~Mesh();
        };

        struct ENGINE_API Model3d final {
            vector<Mesh> Meshes;

            inline Mesh& operator [](u32 i) { return Meshes[i]; }
        };

    }

}
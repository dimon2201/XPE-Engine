#pragma once

#include <geometry/geometry.h>
#include <rendering/materials/material.h>

namespace xpe {

    namespace math {

        struct ENGINE_API Mesh : Geometry<render::Vertex3D> {
            u32 MaterialIndex = 0;
            u32 LightIndex = 0;
        };

        struct ENGINE_API Model3D final {
            vector<Mesh> Meshes;

            inline Mesh& operator [](u32 i) { return Meshes[i]; }
        };

    }

}
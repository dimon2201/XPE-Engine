#pragma once

#include <geometry/geometries.h>

#include <rendering/materials/material.h>

namespace xpe {

    namespace anim {

        using namespace render;
        using namespace math;

        struct ENGINE_API Skin : GeometryIndexed<SkeletalVertex>
        {
            Ref<Material> Material;

            Skin() = default;
            Skin(usize vertexCount, usize indexCount) : GeometryIndexed<SkeletalVertex>(vertexCount, indexCount) {}
        };

        Json(Skin, PrimitiveTopology, Vertices, Indices.List)

        struct ENGINE_API SkinModel : public Object
        {
            ePrimitiveTopology PrimitiveTopology = ePrimitiveTopology::TRIANGLE_LIST;
            vector<Skin> Skins;
            render::VertexBuffer<SkeletalVertex> Vertices;
            render::IndexBuffer Indices;

            inline Skin& operator [](u32 i) { return Skins[i]; }
        };

        Json(SkinModel, PrimitiveTopology, Skins)

    }

}
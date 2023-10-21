#pragma once

#include <geometry/geometries.h>

#include <rendering/materials/material.h>

namespace xpe {

    namespace anim {

        using namespace render;
        using namespace math;

        struct ENGINE_API Skin : Geometry<VertexSkeletal>
        {
            Ref<Material> Material;
        };

        Json(Skin, PrimitiveTopology, VertexOffset, Vertices, IndexOffset, Indices)

        struct ENGINE_API SkinModel : public Object
        {
            ePrimitiveTopology PrimitiveTopology = ePrimitiveTopology::TRIANGLE_LIST;
            vector<Skin> Skins;
            usize VertexOffset = 0;
            usize VertexCount = 0;
            usize IndexOffset = 0;
            vector<u32> Indices;

            inline Skin& operator [](u32 i) { return Skins[i]; }
        };

        Json(SkinModel, PrimitiveTopology, Skins)

    }

}
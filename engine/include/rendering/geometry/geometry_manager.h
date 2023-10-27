#pragma once

#include <rendering/geometry/geometries.h>

namespace xpe {

    namespace render {

        struct ENGINE_API GeometryManager : public Object
        {
            static void Init();
            static void Free();
            static void Bind();

            static Ref<Geometry> AddGeometry(const Geometry& geometry);
            static Ref<Model> AddModel(const Model& model);

        private:
            static usize AddIndices(const vector<u32>& indices);

            static VertexBuffer* s_VertexBuffer;
            static IndexBuffer* s_IndexBuffer;
        };

    }

}
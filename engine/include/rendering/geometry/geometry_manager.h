#pragma once

#include <rendering/geometry/geometries.h>

namespace xpe {

    namespace render {

        class ENGINE_API GeometryManager final
        {

        public:
            static void Init();
            static void Free();
            static void Bind();

            static Geometry AddGeometry(const Geometry& _geometry);
            static Model AddModel(const Model& _model);

        private:
            static usize AddIndices(const vector<u32>& indices);

            static VertexBuffer* s_VertexBuffer;
            static IndexBuffer* s_IndexBuffer;
        };

    }

}
#pragma once

#include <rendering/geometry/geometry.h>

namespace xpe {

    namespace render {

        struct ENGINE_API GeometryManager : public Object
        {
            static void Init();
            static void Free();

            static void BindVertexBuffer();
            static void BindIndexBuffer();

            static Ref<Geometry> AddGeometry(const Geometry& geometry);
            static Ref<Model> AddModel(const Model& model);

        private:
            static usize AddIndices(const vector<u32>& indices);

            static VertexBuffer* s_VertexBuffer;
            static IndexBuffer* s_IndexBuffer;
        };

        Ref<Geometry> GeometryManager::AddGeometry(const Geometry& geometry)
        {
            Ref<Geometry> geometryRef;
            geometryRef.Create(geometry);

            geometryRef->IndexOffset = AddIndices(geometry.Indices);
            geometryRef->VertexOffset = s_VertexBuffer->AddVertices(geometry.Vertices);
            s_VertexBuffer->Flush();

            return geometryRef;
        }

    }

}
#pragma once

#include <anim/skin.h>

namespace xpe {

    namespace render {

        using namespace anim;

        struct ENGINE_API GeometryManager : public Object
        {
            static void Init();
            static void Free();

            static void BindVertexBuffer2D();
            static void BindVertexBuffer3D();
            static void BindVertexBufferSkeletal();
            static void BindIndexBuffer();

            template<typename V>
            static Ref<Geometry<V>> AddGeometry(const Geometry<V>& geometry);
            static Ref<Model3D> AddModel(const Model3D& model);
            static Ref<SkinModel> AddSkinModel(const SkinModel& skinModel);

        private:
            static usize AddIndices(const vector<u32>& indices);

            static VertexBuffer<Vertex2D>* s_VertexBuffer2D;
            static VertexBuffer<Vertex3D>* s_VertexBuffer3D;
            static VertexBuffer<VertexSkeletal>* s_VertexBufferSkeletal;
            static IndexBuffer* s_IndexBuffer;
        };

        template<typename V>
        Ref<Geometry<V>> GeometryManager::AddGeometry(const Geometry<V>& geometry)
        {
            Ref<Geometry<V>> geometryRef;
            geometryRef.Create(geometry);

            geometryRef->IndexOffset = AddIndices(geometry.Indices);

            if constexpr(std::is_same_v<V, Vertex2D>) {
                geometryRef->VertexOffset = s_VertexBuffer2D->AddVertices(geometry.Vertices);
                s_VertexBuffer2D->Flush();
            }

            else if constexpr(std::is_same_v<V, Vertex3D>) {
                geometryRef->VertexOffset = s_VertexBuffer3D->AddVertices(geometry.Vertices);
                s_VertexBuffer3D->Flush();
            }

            else if constexpr(std::is_same_v<V, VertexSkeletal>) {
                geometryRef->VertexOffset = s_VertexBufferSkeletal->AddVertices(geometry.Vertices);
                s_VertexBufferSkeletal->Flush();
            }

            return geometryRef;
        }

    }

}
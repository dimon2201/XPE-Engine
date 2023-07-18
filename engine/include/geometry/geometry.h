#pragma once

#include <rendering/vertex.h>
#include <rendering/core.h>

namespace xpe {

    namespace math {

        using namespace core;

        template<typename T>
        struct VertexArray final {

            vector<T> Data;

            inline usize Size() const { return Data.size() * sizeof(T); }

            inline usize Count() const { return Data.size(); }

            inline float* ToFloat() const { return (float*) Data.data(); }

            inline const T* GetData() const { return Data.data(); }

            inline T& operator [](int i) { return Data[i]; }

            inline usize Capacity() const { return Data.capacity(); }

            void Init(usize count);
            void Free();

            void CopyFrom(VertexArray<T>* src);

            void Reserve(usize count);

        };

        template<typename T>
        void VertexArray<T>::Init(usize count) {
            Data.resize(count);
        }

        template<typename T>
        void VertexArray<T>::Free() {
            Data.clear();
        }

        template<typename T>
        void VertexArray<T>::CopyFrom(VertexArray<T>* src) {
            Data.resize(src->Size());
            memcpy(Data.data(), src->Data.data(), src->Size());
        }

        template<typename T>
        void VertexArray<T>::Reserve(usize count) {
            Data.reserve(count);
        }

        template<typename T>
        static void InitUV(T* v0, T* v1, T* v2, T* v3) {
            v0->UV = { 0, 0 };
            v1->UV = { 0, 1 };
            v2->UV = { 1, 1 };
            v3->UV = { 1, 0 };
        }

        template<typename T>
        static void InitNormal(T* v0, T* v1, T* v2) {
            glm::vec3 x1 = v1->Position - v0->Position;
            glm::vec3 x2 = v2->Position - v0->Position;
            glm::vec3 normal = -glm::normalize(glm::cross(x1, x2));
            v0->Normal = normal;
            v1->Normal = normal;
            v2->Normal = normal;
        }

        template<typename T>
        static void InitNormal(T* v0, T* v1, T* v2, T* v3) {
            glm::vec3 x1 = v1->Position - v0->Position;
            glm::vec3 x2 = v3->Position - v0->Position;
            glm::vec3 normal = -glm::normalize(glm::cross(x1, x2));
            v0->Normal = normal;
            v1->Normal = normal;
            v2->Normal = normal;
            v3->Normal = normal;
        }

        template<typename T>
        static void InitTBN(T* v0, T* v1, T* v2, T* v3) {
            glm::vec3& pos0 = v0->Position;
            glm::vec3& pos1 = v1->Position;
            glm::vec3& pos2 = v2->Position;

            glm::vec2& uv0 = v0->UV;
            glm::vec2& uv1 = v1->UV;
            glm::vec2& uv2 = v2->UV;

            // Calculate Triangle edges
            glm::vec3 dpos_1 = pos1 - pos0;
            glm::vec3 dpos_2 = pos2 - pos0;

            // Calculate delta UV
            glm::vec2 duv_1 = uv1 - uv0;
            glm::vec2 duv_2 = uv2 - uv0;

            // Calculate Tangents
            float r = 1.0f / (duv_1.x * duv_2.y - duv_1.y * duv_2.x);
            glm::vec3 tangent = (dpos_1 * duv_2.y - dpos_2 * duv_1.y) * r;
            v0->Tangent = tangent;
            v1->Tangent = tangent;
            v2->Tangent = tangent;
            v3->Tangent = tangent;
        }

        struct ENGINE_API IndexArray final {
            vector<u32> Data;

            inline usize Size() const { return Data.size() * sizeof(u32); }

            inline usize Count() const { return Data.size(); }

            inline const u32* GetData() const { return Data.data(); }

            inline u32& operator [](int i) { return Data[i]; }

            inline usize Capacity() const { return Data.capacity(); }

            void Init(usize count);
            void Free();

            void CopyFrom(IndexArray* src);
            void CopyFrom(u32* indices, usize count);

            void Reserve(usize count);
        };

        struct ENGINE_API GeometryIndexedFormat final {
            render::ePrimitiveTopology PrimitiveTopology = render::ePrimitiveTopology::DEFAULT;
            string USID;
            usize VertexOffset = 0;
            usize IndexOffset = 0;
            usize VertexCount = 0;
            usize IndexCount = 0;

            GeometryIndexedFormat() = default;

            GeometryIndexedFormat(const render::ePrimitiveTopology& primitiveTopology, usize vertexOffset, usize indexOffset, usize vertexCount, usize indexCount)
            : PrimitiveTopology(primitiveTopology),
            VertexOffset(vertexOffset),
            IndexOffset(indexOffset),
            VertexCount(vertexCount),
            IndexCount(indexCount)
            {}
        };

        template<typename T>
        struct GeometryIndexed {
            VertexArray<T> Vertices;
            IndexArray Indices;
            render::ePrimitiveTopology PrimitiveTopology = render::ePrimitiveTopology::DEFAULT;
        };

        struct ENGINE_API GeometryVertexedFormat final {
            render::ePrimitiveTopology PrimitiveTopology = render::ePrimitiveTopology::DEFAULT;
            string USID;
            usize VertexOffset = 0;
            usize VertexCount = 0;

            GeometryVertexedFormat() = default;

            GeometryVertexedFormat(const render::ePrimitiveTopology& primitiveTopology, usize vertexOffset, usize vertexCount)
            : PrimitiveTopology(primitiveTopology),
            VertexOffset(vertexOffset),
            VertexCount(vertexCount)
            {}
        };

        template<typename T>
        struct GeometryVertexed {
            VertexArray<T> Vertices;
            render::ePrimitiveTopology PrimitiveTopology = render::ePrimitiveTopology::DEFAULT;
        };

    }

}
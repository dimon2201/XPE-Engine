#pragma once

#include <rendering/core/vertex.h>
#include <rendering/core/core.h>

namespace xpe {

    namespace math {

        using namespace core;

        template<typename T>
        struct VertexArray final
        {
            vector<T> Data;

            VertexArray() = default;

            VertexArray(usize count) { Init(count); }

            [[nodiscard]] inline usize Size() const { return Data.size() * sizeof(T); }

            [[nodiscard]] inline usize Count() const { return Data.size(); }

            [[nodiscard]] inline float* ToFloat() const { return (float*) Data.data(); }

            inline const T* GetData() const { return Data.data(); }

            inline T& operator [](int i) { return Data[i]; }

            [[nodiscard]] inline usize Capacity() const { return Data.capacity(); }

            void Init(usize count);
            void Reserve(usize count);
        };

        template<typename T>
        void VertexArray<T>::Init(usize count)
        {
            Data.resize(count);
        }

        template<typename T>
        void VertexArray<T>::Reserve(usize count)
        {
            Data.reserve(count);
        }

        template<typename T>
        static void InitUV(T* v0, T* v1, T* v2, T* v3)
        {
            v0->UV = { 0, 0 };
            v1->UV = { 0, 1 };
            v2->UV = { 1, 1 };
            v3->UV = { 1, 0 };
        }

        template<typename T>
        static void InitNormal(T* v0, T* v1, T* v2)
        {
            glm::vec3 x1 = v1->Position - v0->Position;
            glm::vec3 x2 = v2->Position - v0->Position;
            glm::vec3 normal = -glm::normalize(glm::cross(x1, x2));
            v0->Normal = normal;
            v1->Normal = normal;
            v2->Normal = normal;
        }

        template<typename T>
        static void InitNormal(T* v0, T* v1, T* v2, T* v3)
        {
            glm::vec3 x1 = v1->Position - v0->Position;
            glm::vec3 x2 = v3->Position - v0->Position;
            glm::vec3 normal = -glm::normalize(glm::cross(x1, x2));
            v0->Normal = normal;
            v1->Normal = normal;
            v2->Normal = normal;
            v3->Normal = normal;
        }

        template<typename T>
        static void InitTBN(T* v0, T* v1, T* v2, T* v3)
        {
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

        struct ENGINE_API IndexArray final
        {
            vector<u32> Data;

            IndexArray() = default;

            IndexArray(usize count) { Init(count); }

            [[nodiscard]] inline usize Size() const { return Data.size() * sizeof(u32); }

            [[nodiscard]] inline usize Count() const { return Data.size(); }

            [[nodiscard]] inline const u32* GetData() const { return Data.data(); }

            inline u32& operator [](int i) { return Data[i]; }

            [[nodiscard]] inline usize Capacity() const { return Data.capacity(); }

            void Init(usize count);
            void Reserve(usize count);
        };

        template<typename T>
        struct GeometryIndexed : public Object
        {
            render::ePrimitiveTopology PrimitiveTopology = render::ePrimitiveTopology::DEFAULT;
            VertexArray<T> Vertices;
            IndexArray Indices;

            GeometryIndexed() = default;
            GeometryIndexed(usize vertexCount, usize indexCount) : Vertices(vertexCount), Indices(indexCount) {}
        };

        template<typename T>
        struct GeometryVertexed : public Object
        {
            render::ePrimitiveTopology PrimitiveTopology = render::ePrimitiveTopology::DEFAULT;
            VertexArray<T> Vertices;

            GeometryVertexed() = default;
            GeometryVertexed(usize vertexCount) : Vertices(vertexCount) {}
        };

    }

}
#pragma once

#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace math;

        template<typename T>
        class VertexBuffer : public Buffer
        {

        public:
            vector<T> List;

            VertexBuffer() = default;
            VertexBuffer(const usize vertexCount);
            VertexBuffer(const vector<T>& vertices);
            ~VertexBuffer();

        public:
            void Flush();

            void FlushVertex(u32 index, const T& item);

            void FlushVertices(const vector<T>& vertices);

            void Resize(const usize count);

            void Reserve(const usize count);

            void Recreate(const usize count);

            void Clear();

            inline T* GetVertex(const u32 index)
            {
                // check if index is in the size bounds
                // if not, then resize to index + 1
                if (index >= Count()) {
                    Resize(index + 1);
                }
                return &List[index];
            }

            inline usize Count()
            {
                return List.size();
            }

        };

        template<typename T>
        VertexBuffer<T>::VertexBuffer(const usize vertexCount)
        {
            Type = eBufferType::VERTEX;
            StructureSize = sizeof(T);
            NumElements = vertexCount;
            context::CreateBuffer(*this);
            List.resize(vertexCount);
        }

        template<typename T>
        VertexBuffer<T>::VertexBuffer(const vector<T>& vertexArray) : List(vertexArray)
        {
            Type = eBufferType::VERTEX;
            StructureSize = sizeof(T);
            NumElements = vertexArray.size();
            context::CreateBuffer(*this);
            FlushVertices(vertexArray);
        }

        template<typename T>
        VertexBuffer<T>::~VertexBuffer()
        {
            context::FreeBuffer(*this);
        }

        template<typename T>
        void VertexBuffer<T>::Flush()
        {
            usize size = List.size();
            if (size != NumElements) {
                Recreate(size);
            }
            else {
                context::CopyBuffer(*this, List.data(), ByteSize());
            }
        }

        template<typename T>
        void VertexBuffer<T>::FlushVertex(u32 index, const T& vertex)
        {
            if (index >= List.size()) {
                Resize(index + 1);
            }
            List[index] = vertex;
            context::MoveBufferOffset(*this, StructureSize * index, &List.back(), StructureSize);
        }

        template<typename T>
        void VertexBuffer<T>::FlushVertices(const vector<T> &vertices)
        {
            if (sizeof(vertices) > ByteSize()) {
                Resize(vertices.size());
            }
            memcpy((void*)List.data(), (const void*)vertices.data(), sizeof(vertices));
            Flush();
        }

        template<typename T>
        void VertexBuffer<T>::Recreate(const usize vertexCount)
        {
            Type = eBufferType::VERTEX;
            NumElements = vertexCount;
            StructureSize = sizeof(T);
            context::FreeBuffer(*this);
            context::CreateBuffer(*this);
            context::CopyBuffer(*this, List.data(), ByteSize());
        }

        template<typename T>
        void VertexBuffer<T>::Resize(const usize vertexCount)
        {
            List.resize(vertexCount);
        }

        template<typename T>
        void VertexBuffer<T>::Reserve(const usize vertexCount)
        {
            List.reserve(vertexCount);
        }

        template<typename T>
        void VertexBuffer<T>::Clear()
        {
            List.clear();
        }

    }

}
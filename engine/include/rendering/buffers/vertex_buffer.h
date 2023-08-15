#pragma once

#include <geometry/geometry.h>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace math;

        class Context;

        template<typename T>
        class VertexBuffer : public Buffer
        {

        public:
            VertexBuffer() = default;
            VertexBuffer(const usize vertexCount);
            VertexBuffer(const VertexArray<T>& vertexArray);
            ~VertexBuffer();

        public:
            void Flush();

            void FlushVertex(u32 index, const T& item);

            void FlushVertices(const VertexArray<T>& vertices);

            void Resize(const usize count);

            void Reserve(const usize count);

            void Recreate(const usize count);

            inline T* GetVertex(const u32 index)
            {
                // check if index is in the size bounds
                // if not, then resize to index + 1
                if (index >= Count()) {
                    Resize(index + 1);
                }
                return &m_VertexArray[index];
            }

            inline VertexArray<T>& GetVertexArray()
            {
                return m_VertexArray;
            }

            inline usize Count()
            {
                return m_VertexArray.Count();
            }

        protected:
            VertexArray<T> m_VertexArray;
        };

        template<typename T>
        VertexBuffer<T>::VertexBuffer(const usize vertexCount)
        {
            Type = eBufferType::VERTEX;
            StructureSize = sizeof(T);
            NumElements = vertexCount;
            context::CreateBuffer(*this);
            m_VertexArray.Init(vertexCount);
        }

        template<typename T>
        VertexBuffer<T>::VertexBuffer(const VertexArray<T>& vertexArray) : m_VertexArray(vertexArray)
        {
            Type = eBufferType::VERTEX;
            StructureSize = sizeof(T);
            NumElements = vertexArray.Count();
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
            context::CopyBuffer(*this, m_VertexArray.GetData(), m_VertexArray.Size());
        }

        template<typename T>
        void VertexBuffer<T>::FlushVertex(u32 index, const T& vertex)
        {
            if (index >= m_VertexArray.Count()) {
                Resize(index + 1);
            }
            m_VertexArray[index] = vertex;
            context::MoveBufferOffset(*this, StructureSize * index, &m_VertexArray.Data.back(), StructureSize);
        }

        template<typename T>
        void VertexBuffer<T>::FlushVertices(const VertexArray<T> &vertices)
        {
            if (vertices.Size() > ByteSize()) {
                Resize(vertices.Count());
            }
            memcpy((void*)m_VertexArray.GetData(), (const void*)vertices.GetData(), vertices.Size());
            Flush();
        }

        template<typename T>
        void VertexBuffer<T>::Recreate(const usize vertexCount)
        {
            NumElements = vertexCount;
            context::FreeBuffer(*this);
            context::CreateBuffer(*this);
            Flush();
        }

        template<typename T>
        void VertexBuffer<T>::Resize(const usize vertexCount)
        {
            usize capacity = m_VertexArray.Capacity();
            m_VertexArray.Init(vertexCount);
            if (capacity < vertexCount * StructureSize) {
                Recreate(vertexCount);
            }
        }

        template<typename T>
        void VertexBuffer<T>::Reserve(const usize vertexCount)
        {
            usize capacity = m_VertexArray.Capacity();
            m_VertexArray.Reserve(vertexCount);
            if (capacity < vertexCount * StructureSize) {
                Recreate(vertexCount);
            }
        }

    }

}
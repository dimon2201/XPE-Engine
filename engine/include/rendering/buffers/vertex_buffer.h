#pragma once

#include <rendering/core/context.hpp>
#include <rendering/core/vertex.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace math;

        class VertexBuffer : public Buffer
        {

        public:
            vector<Vertex> List;

            VertexBuffer() = default;
            VertexBuffer(const usize vertexCount);
            VertexBuffer(const vector<Vertex>& vertices);
            ~VertexBuffer();

        public:
            void Flush();

            void FlushVertex(u32 index, const Vertex& item);

            void FlushVertices(const vector<Vertex>& vertices);

            void Resize(const usize count);

            void Reserve(const usize count);

            void Recreate(const usize count);

            void Clear();

            // returns vertex offset of added vertices
            usize AddVertices(const vector<Vertex>& vertices);

            inline Vertex* GetVertex(const u32 index)
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

        VertexBuffer::VertexBuffer(const usize vertexCount)
        {
            Type = eBufferType::VERTEX;
            StructureSize = sizeof(T);
            NumElements = vertexCount;
            List.resize(vertexCount);
            InitialData = List.data();
            context::CreateBuffer(*this);
        }

        VertexBuffer::VertexBuffer(const vector<Vertex>& vertexArray) : List(vertexArray)
        {
            Type = eBufferType::VERTEX;
            StructureSize = sizeof(T);
            NumElements = vertexArray.size();
            InitialData = List.data();
            context::CreateBuffer(*this);
            Flush();
        }

        VertexBuffer::~VertexBuffer()
        {
            context::FreeBuffer(*this);
        }

        void VertexBuffer::Flush()
        {
            usize size = List.size();
            if (size != NumElements) {
                Recreate(size);
            }
            else {
                context::CopyBuffer(*this, List.data(), GetByteSize());
            }
        }

        void VertexBuffer::FlushVertex(u32 index, const Vertex& vertex)
        {
            if (index >= List.size()) {
                Resize(index + 1);
            }
            List[index] = vertex;
            context::MoveBufferOffset(*this, StructureSize * index, &List.back(), StructureSize);
        }

        void VertexBuffer::FlushVertices(const vector<Vertex> &vertices)
        {
            if (sizeof(vertices) > GetByteSize()) {
                Resize(vertices.size());
            }
            memcpy((void*)List.data(), (const void*)vertices.data(), sizeof(vertices));
            Flush();
        }

        void VertexBuffer::Recreate(const usize vertexCount)
        {
            Type = eBufferType::VERTEX;
            NumElements = vertexCount;
            StructureSize = sizeof(T);
            InitialData = List.data();
            context::FreeBuffer(*this);
            context::CreateBuffer(*this);
            context::CopyBuffer(*this, List.data(), GetByteSize());
        }

        void VertexBuffer::Resize(const usize vertexCount)
        {
            List.resize(vertexCount);
        }

        void VertexBuffer::Reserve(const usize vertexCount)
        {
            List.reserve(vertexCount);
        }

        void VertexBuffer::Clear()
        {
            List.clear();
        }

        usize VertexBuffer::AddVertices(const vector<Vertex>& vertices)
        {
            usize vertexOffset = List.size();
            List.resize(List.size() + vertices.size());
            memcpy(&List[vertexOffset], vertices.data(), vertices.size() * sizeof(T));
            return vertexOffset;
        }

    }

}
#include <rendering/buffers/vertex_buffer.h>

namespace xpe {

    namespace render {

        VertexBuffer::VertexBuffer(const usize vertexCount)
        {
            Type = eBufferType::VERTEX;
            StructureSize = sizeof(Vertex);
            NumElements = vertexCount;
            List.resize(vertexCount);
            InitialData = List.data();
            context::CreateBuffer(*this);
        }

        VertexBuffer::VertexBuffer(const vector<Vertex>& vertexArray) : List(vertexArray)
        {
            Type = eBufferType::VERTEX;
            StructureSize = sizeof(Vertex);
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
            StructureSize = sizeof(Vertex);
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
            memcpy(&List[vertexOffset], vertices.data(), vertices.size() * sizeof(Vertex));
            return vertexOffset;
        }

    }

}
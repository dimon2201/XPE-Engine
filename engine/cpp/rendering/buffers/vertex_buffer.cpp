#include <rendering/buffers/vertex_buffer.h>

namespace xpe {

    namespace render {

        sVertexBuffer::sVertexBuffer(const usize vertexCount, sBuffer::eSubType subtype, sBuffer::eViewType viewtype)
        {
            Type = sBuffer::eType::VERTEX;
            SubType = subtype;
            ViewType = viewtype;
            StructureSize = sizeof(sVertex);
            NumElements = vertexCount;
            if (vertexCount > 0) {
                List.resize(vertexCount);
                InitialData = List.data();
                context::CreateBuffer(*this);
            }
        }

        sVertexBuffer::sVertexBuffer(const vector<sVertex>& vertexArray, sBuffer::eSubType subtype, sBuffer::eViewType viewtype)
        : List(vertexArray)
        {
            Type = sBuffer::eType::VERTEX;
            SubType = subtype;
            ViewType = viewtype;
            StructureSize = sizeof(sVertex);
            NumElements = vertexArray.size();
            if (vertexArray.size() > 0) {
                InitialData = List.data();
                context::CreateBuffer(*this);
                Flush();
            }
        }

        sVertexBuffer::~sVertexBuffer()
        {
            context::FreeBuffer(*this);
        }

        void sVertexBuffer::Flush()
        {
            usize size = List.size();
            if (size != NumElements) {
                Recreate(size);
            }
            else {
                context::CopyBuffer(*this, List.data(), GetByteSize());
            }
        }

        void sVertexBuffer::FlushVertex(u32 index, const sVertex& vertex)
        {
            if (index >= List.size()) {
                Resize(index + 1);
            }
            List[index] = vertex;
            context::MoveBufferOffset(*this, StructureSize * index, &List.back(), StructureSize);
        }

        void sVertexBuffer::FlushVertices(const vector<sVertex> &vertices)
        {
            if (sizeof(vertices) > GetByteSize()) {
                Resize(vertices.size());
            }
            memcpy((void*)List.data(), (const void*)vertices.data(), sizeof(vertices));
            Flush();
        }

        void sVertexBuffer::Recreate(const usize vertexCount)
        {
            NumElements = vertexCount;
            StructureSize = sizeof(sVertex);
            InitialData = List.data();
            context::FreeBuffer(*this);
            context::CreateBuffer(*this);
            context::CopyBuffer(*this, List.data(), GetByteSize());
        }

        void sVertexBuffer::Resize(const usize vertexCount)
        {
            List.resize(vertexCount);
        }

        void sVertexBuffer::Reserve(const usize vertexCount)
        {
            List.reserve(vertexCount);
        }

        void sVertexBuffer::Clear()
        {
            List.clear();
        }

        usize sVertexBuffer::AddVertices(const vector<sVertex>& vertices)
        {
            usize vertexOffset = List.size();
            List.resize(List.size() + vertices.size());
            memcpy(&List[vertexOffset], vertices.data(), vertices.size() * sizeof(sVertex));
            return vertexOffset;
        }

    }

}
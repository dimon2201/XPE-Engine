#include <rendering/buffers.hpp>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        sIndexBuffer::sIndexBuffer(const usize indexCount, sBuffer::eSubType subtype, sBuffer::eViewType viewtype)
        {
            Type = sBuffer::eType::INDEX;
            SubType = subtype;
            ViewType = viewtype;
            StructureSize = sizeof(u32);
            NumElements = indexCount;
            if (indexCount > 0) {
                List.resize(indexCount);
                InitialData = List.data();
                context::CreateBuffer(*this);
            }
        }

        sIndexBuffer::sIndexBuffer(const vector<u32>& indexArray, sBuffer::eSubType subtype, sBuffer::eViewType viewtype)
                : List(indexArray)
        {
            Type = sBuffer::eType::INDEX;
            SubType = subtype;
            ViewType = viewtype;
            StructureSize = sizeof(u32);
            NumElements = indexArray.size();
            if (indexArray.size() > 0) {
                InitialData = List.data();
                context::CreateBuffer(*this);
                Flush();
            }
        }

        sIndexBuffer::~sIndexBuffer()
        {
            context::FreeBuffer(*this);
        }

        void sIndexBuffer::Flush()
        {
            usize size = List.size();
            if (size != NumElements) {
                Recreate(size);
            }
            else {
                context::CopyBuffer(*this, List.data(), GetByteSize());
            }
        }

        void sIndexBuffer::FlushIndices(const vector<u32> &indices)
        {
            if (sizeof(indices) > GetByteSize()) {
                Resize(indices.size());
            }
            memcpy((void*)List.data(), (const void*)indices.data(), indices.size());
            Flush();
        }

        void sIndexBuffer::FlushIndex(u32 i, u32 index)
        {
            if (i >= List.size()) {
                Resize(i + 1);
            }
            List[i] = index;
            context::MoveBufferOffset(*this, StructureSize * i, &List.back(), StructureSize);
        }

        void sIndexBuffer::Recreate(const usize indexCount)
        {
            NumElements = indexCount;
            StructureSize = sizeof(u32);
            InitialData = List.data();
            context::FreeBuffer(*this);
            context::CreateBuffer(*this);
            context::CopyBuffer(*this, List.data(), GetByteSize());
        }

        usize sIndexBuffer::AddIndices(const vector<u32>& indices)
        {
            usize indexOffset = List.size();
            List.resize(List.size() + indices.size());
            memcpy(&List[indexOffset], indices.data(), indices.size() * sizeof(u32));
            return indexOffset;
        }

        void sIndexBuffer::Resize(const usize indexCount)
        {
            List.resize(indexCount);
        }

        void sIndexBuffer::Reserve(const usize indexCount)
        {
            List.reserve(indexCount);
        }

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
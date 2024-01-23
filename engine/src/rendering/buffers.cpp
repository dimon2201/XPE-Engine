#include <rendering/buffers.hpp>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        cIndexBuffer::cIndexBuffer(const usize indexCount, cBuffer::eViewType viewtype)
        {
            m_Type = cBuffer::eType::INDEX;
            m_ViewType = viewtype;
            m_StructureSize = sizeof(u32);
            m_NumElements = indexCount;
            if (indexCount > 0) {
                m_List.resize(indexCount);
                m_InitialData = m_List.data();
                context::CreateBuffer(*this);
            }
        }

        cIndexBuffer::cIndexBuffer(const vector<u32>& indexArray, cBuffer::eViewType viewtype) : m_List(indexArray)
        {
            m_Type = cBuffer::eType::INDEX;
            m_ViewType = viewtype;
            m_StructureSize = sizeof(u32);
            m_NumElements = indexArray.size();
            if (indexArray.size() > 0) {
                m_InitialData = m_List.data();
                context::CreateBuffer(*this);
                Flush();
            }
        }

        cIndexBuffer::~cIndexBuffer()
        {
            context::FreeBuffer(*this);
        }

        void cIndexBuffer::Flush()
        {
            usize size = m_List.size();
            if (size != m_NumElements) {
                Recreate(size);
            } else {
                context::WriteBuffer(*this, m_List.data(), GetByteSize());
            }
        }

        void cIndexBuffer::FlushIndices(const vector<u32> &indices)
        {
            if (sizeof(indices) > GetByteSize()) {
                Resize(indices.size());
            }
            memcpy((void*)m_List.data(), (const void*)indices.data(), indices.size());
            Flush();
        }

        void cIndexBuffer::FlushIndex(u32 i, u32 index)
        {
            if (i >= m_List.size()) {
                Resize(i + 1);
            }
            m_List[i] = index;
            context::MoveBufferOffset(*this, m_StructureSize * i, &m_List.back(), m_StructureSize);
        }

        void cIndexBuffer::Recreate(const usize indexCount)
        {
            m_NumElements = indexCount;
            m_StructureSize = sizeof(u32);
            m_InitialData = m_List.data();
            context::FreeBuffer(*this);
            context::CreateBuffer(*this);
            context::WriteBuffer(*this, m_List.data(), GetByteSize());
        }

        usize cIndexBuffer::AddIndices(const vector<u32>& indices)
        {
            usize indexOffset = m_List.size();
            m_List.resize(m_List.size() + indices.size());
            memcpy(&m_List[indexOffset], indices.data(), indices.size() * sizeof(u32));
            return indexOffset;
        }

        void cIndexBuffer::Resize(const usize indexCount)
        {
            m_List.resize(indexCount);
        }

        void cIndexBuffer::Reserve(const usize indexCount)
        {
            m_List.reserve(indexCount);
        }

        cVertexBuffer::cVertexBuffer(const usize vertexCount, cBuffer::eViewType viewType)
        {
            m_Type = cBuffer::eType::VERTEX;
            m_ViewType = viewType;
            m_StructureSize = sizeof(sVertex);
            m_NumElements = vertexCount;
            if (vertexCount > 0) {
                m_List.resize(vertexCount);
                m_InitialData = m_List.data();
                context::CreateBuffer(*this);
            }
        }

        cVertexBuffer::cVertexBuffer(const vector<sVertex>& vertexArray, cBuffer::eViewType viewtype)
                : m_List(vertexArray)
        {
            m_Type = cBuffer::eType::VERTEX;
            m_ViewType = viewtype;
            m_StructureSize = sizeof(sVertex);
            m_NumElements = vertexArray.size();
            if (vertexArray.size() > 0) {
                m_InitialData = m_List.data();
                context::CreateBuffer(*this);
                Flush();
            }
        }

        cVertexBuffer::~cVertexBuffer()
        {
            context::FreeBuffer(*this);
        }

        void cVertexBuffer::Flush()
        {
            usize size = m_List.size();
            if (size != m_NumElements) {
                Recreate(size);
            } else {
                context::WriteBuffer(*this, m_List.data(), GetByteSize());
            }
        }

        void cVertexBuffer::FlushVertex(u32 index, const sVertex& vertex)
        {
            if (index >= m_List.size()) {
                Resize(index + 1);
            }
            m_List[index] = vertex;
            context::MoveBufferOffset(*this, m_StructureSize * index, &m_List.back(), m_StructureSize);
        }

        void cVertexBuffer::FlushVertices(const vector<sVertex> &vertices)
        {
            if (sizeof(vertices) > GetByteSize()) {
                Resize(vertices.size());
            }
            memcpy((void*)m_List.data(), (const void*)vertices.data(), sizeof(vertices));
            Flush();
        }

        void cVertexBuffer::Recreate(const usize vertexCount)
        {
            m_NumElements = vertexCount;
            m_StructureSize = sizeof(sVertex);
            m_InitialData = m_List.data();
            context::FreeBuffer(*this);
            context::CreateBuffer(*this);
            context::WriteBuffer(*this, m_List.data(), GetByteSize());
        }

        void cVertexBuffer::Resize(const usize vertexCount)
        {
            m_List.resize(vertexCount);
        }

        void cVertexBuffer::Reserve(const usize vertexCount)
        {
            m_List.reserve(vertexCount);
        }

        void cVertexBuffer::Clear()
        {
            m_List.clear();
        }

        usize cVertexBuffer::AddVertices(const vector<sVertex>& vertices)
        {
            usize vertexOffset = m_List.size();
            m_List.resize(m_List.size() + vertices.size());
            memcpy(&m_List[vertexOffset], vertices.data(), vertices.size() * sizeof(sVertex));
            return vertexOffset;
        }

    }

}
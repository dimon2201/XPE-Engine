#include <rendering/buffers/index_buffer.h>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        IndexBuffer::IndexBuffer(const usize indexCount)
        {
            m_IndexArray.Init(indexCount);
            Type = eBufferType::INDEX;
            StructureSize = sizeof(u32);
            FirstElement = 0;
            NumElements = indexCount;
            ByteSize = StructureSize * indexCount;
            Duplicate = K_FALSE;
            context::CreateBuffer(*this);
        }

        void IndexBuffer::Free() {
            context::FreeBuffer(*this);
            m_IndexArray.Free();
        }

        void IndexBuffer::Flush() {
            context::WriteBuffer(*this, m_IndexArray.GetData(), m_IndexArray.Size());
        }

        void IndexBuffer::FlushIndices(const IndexArray &indices) {
            if (indices.Size() > ByteSize) {
                Resize(indices.Count());
            }
            memcpy((void*)m_IndexArray.GetData(), (const void*)indices.GetData(), indices.Size());
            Flush();
        }

        void IndexBuffer::Bind() {
            context::BindIndexBuffer(this);
        }

        void IndexBuffer::FlushIndex(u32 i, u32 index) {
            if (i >= m_IndexArray.Count()) {
                Resize(i + 1);
            }
            m_IndexArray[i] = index;
            context::WriteBufferOffset(*this, StructureSize * i, &m_IndexArray.Data.back(), StructureSize);
        }

        void IndexBuffer::Recreate(const usize indexCount) {
            NumElements = indexCount;
            ByteSize = NumElements * StructureSize;
            context::FreeBuffer(*this);
            context::CreateBuffer(*this);
            Flush();
        }

        void IndexBuffer::Resize(const usize indexCount) {
            usize capacity = m_IndexArray.Capacity();
            m_IndexArray.Init(indexCount);
            if (capacity < indexCount * StructureSize) {
                Recreate(indexCount);
            }
        }

        void IndexBuffer::Reserve(const usize indexCount) {
            usize capacity = m_IndexArray.Capacity();
            m_IndexArray.Reserve(indexCount);
            if (capacity < indexCount) {
                Recreate(indexCount);
            }
        }

    }

}
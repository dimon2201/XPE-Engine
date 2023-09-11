#include <rendering/buffers/index_buffer.h>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        IndexBuffer::IndexBuffer(const usize indexCount)
        {
            List.resize(indexCount);
            Type = eBufferType::INDEX;
            StructureSize = sizeof(u32);
            NumElements = indexCount;
            InitialData = List.data();
            context::CreateBuffer(*this);
        }

        IndexBuffer::IndexBuffer(const vector<u32>& indexArray) : List(indexArray)
        {
            Type = eBufferType::INDEX;
            StructureSize = sizeof(u32);
            NumElements = indexArray.size();
            InitialData = List.data();
            context::CreateBuffer(*this);
            Flush();
        }

        IndexBuffer::~IndexBuffer()
        {
            context::FreeBuffer(*this);
        }

        void IndexBuffer::Flush()
        {
            usize size = List.size();
            if (size != NumElements) {
                Recreate(size);
            }
            else {
                context::CopyBuffer(*this, List.data(), ByteSize());
            }
        }

        void IndexBuffer::FlushIndices(const vector<u32> &indices)
        {
            if (sizeof(indices) > ByteSize()) {
                Resize(indices.size());
            }
            memcpy((void*)List.data(), (const void*)indices.data(), indices.size());
            Flush();
        }

        void IndexBuffer::FlushIndex(u32 i, u32 index)
        {
            if (i >= List.size()) {
                Resize(i + 1);
            }
            List[i] = index;
            context::MoveBufferOffset(*this, StructureSize * i, &List.back(), StructureSize);
        }

        void IndexBuffer::Recreate(const usize indexCount)
        {
            Type = eBufferType::INDEX;
            NumElements = indexCount;
            StructureSize = sizeof(u32);
            InitialData = List.data();
            context::FreeBuffer(*this);
            context::CreateBuffer(*this);
            context::CopyBuffer(*this, List.data(), ByteSize());
        }

        void IndexBuffer::Resize(const usize indexCount)
        {
            List.resize(indexCount);
        }

        void IndexBuffer::Reserve(const usize indexCount)
        {
            List.reserve(indexCount);
        }

    }

}
#include <rendering/buffers/index_buffer.h>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        sIndexBuffer::sIndexBuffer(const usize indexCount)
        {
            List.resize(indexCount);
            Type = eBufferType::INDEX;
            StructureSize = sizeof(u32);
            NumElements = indexCount;
            InitialData = List.data();
            context::CreateBuffer(*this);
        }

        sIndexBuffer::sIndexBuffer(const vector<u32>& indexArray) : List(indexArray)
        {
            Type = eBufferType::INDEX;
            StructureSize = sizeof(u32);
            NumElements = indexArray.size();
            InitialData = List.data();
            context::CreateBuffer(*this);
            Flush();
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
            Type = eBufferType::INDEX;
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

    }

}
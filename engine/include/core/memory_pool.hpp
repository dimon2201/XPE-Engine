#pragma once

namespace xpe
{
    namespace core
    {
        struct ENGINE_API MemoryPoolAllocation
        {
            u32 _freeFlag;
            u32 _allocByteWidth;
            u32 _occupiedByteWidth;
            void* _address;
        };

        class ENGINE_API MemoryPool
        {
            public:
                explicit MemoryPool(const usize size);
                ~MemoryPool();

                void* Allocate(const usize size);
                void Free(void* address);
                u32 GetCount();

            private:
                usize _size;
                void* _memory;
                void* _lastAddress;
                void* _maxAddress;
                std::vector<MemoryPoolAllocation> _allocs;
        };

        MemoryPool* GetMemoryPool(s32 index);
    }
}
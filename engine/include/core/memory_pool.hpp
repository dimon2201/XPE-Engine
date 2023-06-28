#pragma once

namespace xpe
{
    namespace core
    {
        struct MemoryPoolAllocation
        {
            u32 _freeFlag;
            u32 _allocByteWidth;
            u32 _occupiedByteWidth;
            void* _address;
        };

        class MemoryPool
        {
            public:
                explicit MemoryPool(const usize size);
                ~MemoryPool();

                void* Allocate(const usize size);
                bool Free(void* address);
                u32 GetCount();

            private:
                usize _size;
                void* _memory;
                void* _lastAddress;
                void* _maxAddress;
                std::vector<MemoryPoolAllocation> _allocs;
        };

        class ENGINE_API MemoryPoolManager final {

        public:
            static MemoryPool* CreatePool(const usize size);
            static void* Allocate(const usize size);
            static void Free(void* address);
            static u32 GetPoolCount();
            static u32 GetAllocCount(u32 index);

        private:
            static std::vector<MemoryPool*> s_memoryPools;
        };
    }
}
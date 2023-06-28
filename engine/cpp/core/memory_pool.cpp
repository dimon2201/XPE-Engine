#include <core/memory_pool.hpp>

namespace xpe {

    namespace core {

        MemoryPool::MemoryPool(const usize size)
        {
            _size = size;
            _memory = malloc(_size);
            _lastAddress = _memory;
            _maxAddress = (void*)(((u64)_memory) + ((u64)size));
        }

        MemoryPool::~MemoryPool()
        {
            free(_memory);
        }

        void* MemoryPool::Allocate(const usize size)
        {
            for (auto& alloc: _allocs)
            {
                if (alloc._freeFlag == 1u && alloc._allocByteWidth >= size)
                {
                    alloc._freeFlag = 255u;
                    alloc._occupiedByteWidth = size;

                    return alloc._address;
                }

                if ((alloc._allocByteWidth - alloc._occupiedByteWidth) >= size)
                {
                    // New allocation
                    MemoryPoolAllocation newAlloc;
                    newAlloc._freeFlag = 0u;
                    newAlloc._allocByteWidth = alloc._allocByteWidth - alloc._occupiedByteWidth;
                    newAlloc._occupiedByteWidth = size;
                    newAlloc._address = (void*)(((u64)alloc._address) + ((u64)alloc._occupiedByteWidth));

                    alloc._allocByteWidth = alloc._occupiedByteWidth;

                    _allocs.push_back(newAlloc);

                    return newAlloc._address;
                }
            }

            if (_lastAddress >= _maxAddress) { return nullptr; }

            MemoryPoolAllocation newAlloc;
            newAlloc._freeFlag = 0u;
            newAlloc._allocByteWidth = size;
            newAlloc._occupiedByteWidth = size;
            newAlloc._address = _lastAddress;
            _allocs.push_back(newAlloc);

            // Update last address
            _lastAddress = (void*)(((u64)_lastAddress) + ((u64)size));

            return newAlloc._address;
        }

        bool MemoryPool::Free(void* address)
        {
            for (auto& alloc: _allocs)
            {
                if (alloc._address == address)
                {
                    alloc._freeFlag = 1u;
                    alloc._occupiedByteWidth = 0;

                    return true;
                }
            }

            return false;
        }

        u32 MemoryPool::GetCount()
        {
            return _allocs.size();
        }

        std::vector<MemoryPool*> MemoryPoolManager::s_memoryPools;

        MemoryPool* MemoryPoolManager::CreatePool(const usize size) {
            auto* newPool = new MemoryPool(size);
            s_memoryPools.push_back(newPool);
            return newPool;
        }

        void* MemoryPoolManager::Allocate(const usize size) {
            void* newAddress = nullptr;

            for (const auto& pool : s_memoryPools) {
                newAddress = pool->Allocate(size);
                if (newAddress != nullptr) {
                    return newAddress;
                }
            }

            if (newAddress == nullptr) {
                usize poolSize = size <= K_MAX_MEMORY_POOL_BYTE_SIZE ? K_MAX_MEMORY_POOL_BYTE_SIZE : size;
                auto* newPool = CreatePool(poolSize);
                newAddress = newPool->Allocate(size);
            }

            return newAddress;
        }

        void MemoryPoolManager::Free(void* address) {
            for (const auto& pool : s_memoryPools) {
                if (pool->Free(address)) {
                    return;
                }
            }
        }

        u32 MemoryPoolManager::GetPoolCount() {
            return s_memoryPools.size();
        }

        u32 MemoryPoolManager::GetAllocCount(u32 index) {
            return s_memoryPools[index]->GetCount();
        }

    }

}
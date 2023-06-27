#include <core/memory_pool.hpp>

xpe::core::MemoryPool::MemoryPool(const usize size)
{
    _size = size;
    _memory = malloc(_size);
    _lastAddress = _memory;
    _maxAddress = (void*)(((u64)_memory) + ((u64)size));
}

xpe::core::MemoryPool::~MemoryPool()
{
    free(_memory);
}

void* xpe::core::MemoryPool::Allocate(const usize size)
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

void xpe::core::MemoryPool::Free(void* address)
{
    for (auto& alloc: _allocs)
    {
        if (alloc._address == address)
        {
            alloc._freeFlag = 1u;
            alloc._occupiedByteWidth = 0;
            
            return;
        }
    }
}

xpe::core::u32 xpe::core::MemoryPool::GetCount()
{
    return _allocs.size();
}

xpe::core::MemoryPool* xpe::core::GetMemoryPool(s32 index)
{
    static std::vector<MemoryPool*> s_memoryPools;

    while (s_memoryPools.size() <= index) {
        s_memoryPools.push_back(new MemoryPool(K_MAX_MEMORY_POOL_BYTE_SIZE));
    }

    return s_memoryPools[index];
}
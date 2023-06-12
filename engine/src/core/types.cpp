#include "types.hpp"
#include "memory_pool.hpp"

void* xpe::core::Object::operator new(size_t byteSize)
{
    MemoryPool* memoryPool = GetMemoryPool(0);

    return memoryPool->Allocate(byteSize);
}

void xpe::core::Object::operator delete(void* address)
{
    MemoryPool* memoryPool = GetMemoryPool(0);

    return memoryPool->Free(address);
}
#include <core/types.hpp>

xpe::core::uword xpe::core::Object::GetType()
{
    return ((uword*)this)[0];
}

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
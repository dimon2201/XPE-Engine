#pragma once

#define halloc(size) xpe::core::MemoryPoolManager::HotPools->Allocate(size)
#define hallocT(T, count) static_cast<T*>(halloc(sizeof(T) * count))
#define dehalloc(addr) xpe::core::MemoryPoolManager::HotPools->Free(addr)

namespace xpe {

    namespace core {



    }

}
#pragma once

#define alloc(size) xpe::core::MemoryPoolManager::MainPools->Allocate(size)
#define allocT(T, count) static_cast<T*>(alloc(sizeof(T) * count))
#define alloc_construct(T) xpe::core::MemoryPoolManager::MainPools->AllocateConstruct<T>()
#define alloc_construct_args(T, ...) xpe::core::MemoryPoolManager::MainPools->AllocateConstructArgs<T>(__VA_ARGS__)
#define dealloc(addr) xpe::core::MemoryPoolManager::MainPools->Free(addr)
#define destruct(T, addr) \
addr->~T();               \
dealloc(addr)

namespace xpe {

    namespace core {

        template<typename T>
        struct MainAllocator
        {

            typedef T value_type;

            MainAllocator() = default;

            template<class U>
            constexpr MainAllocator(const MainAllocator<U>&) noexcept {}

            [[nodiscard]] T* allocate(std::size_t count)
            {
                return static_cast<T*>(alloc(sizeof(T) * count));
            }

            void deallocate(T* element, std::size_t count) noexcept
            {
                dealloc(element);
            }
        };

        template<class T, class U>
        bool operator==(const MainAllocator<T>&, const MainAllocator<U>&) { return true; }

        template<class T, class U>
        bool operator!=(const MainAllocator<T>&, const MainAllocator<U>&) { return false; }

    }

}
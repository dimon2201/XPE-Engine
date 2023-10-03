#pragma once

#define hot_alloc(size) xpe::core::MemoryPoolManager::HotPools->Allocate(size)
#define hot_allocT(T, count) static_cast<T*>(hot_alloc(sizeof(T) * count))
#define hot_construct(T) xpe::core::MemoryPoolManager::HotPools->AllocateConstruct<T>()
#define hot_construct_args(T, ...) xpe::core::MemoryPoolManager::HotPools->AllocateConstructArgs<T>(__VA_ARGS__)
#define hot_free(addr) xpe::core::MemoryPoolManager::HotPools->Free(addr)
#define hot_destruct(T, addr) \
addr->~T();                \
hot_free(addr)

namespace xpe {

    namespace core {

        template<typename T>
        struct HotAllocator
        {

            typedef T value_type;

            HotAllocator() = default;

            template<class U>
            constexpr HotAllocator(const HotAllocator<U>&) noexcept {}

            [[nodiscard]] T* allocate(std::size_t count)
            {
                return static_cast<T*>(hot_alloc(sizeof(T) * count));
            }

            void deallocate(T* element, std::size_t count) noexcept
            {
                hot_free(element);
            }
        };

        template<class T, class U>
        bool operator==(const HotAllocator<T>&, const HotAllocator<U>&) { return true; }

        template<class T, class U>
        bool operator!=(const HotAllocator<T>&, const HotAllocator<U>&) { return false; }

    }

}
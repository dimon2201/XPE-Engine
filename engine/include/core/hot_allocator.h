#pragma once

#define halloc(size) xpe::core::MemoryPoolManager::HotPools->Allocate(size)
#define hallocT(T, count) static_cast<T*>(halloc(sizeof(T) * count))
#define halloc_construct(T) xpe::core::MemoryPoolManager::HotPools->AllocateConstruct<T>()
#define halloc_construct_args(T, ...) xpe::core::MemoryPoolManager::HotPools->AllocateConstructArgs<T>(__VA_ARGS__)
#define dehalloc(addr) xpe::core::MemoryPoolManager::HotPools->Free(addr)

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
                return static_cast<T*>(halloc(sizeof(T) * count));
            }

            void deallocate(T* element, std::size_t count) noexcept
            {
                dehalloc(element);
            }
        };

        template<class T, class U>
        bool operator==(const HotAllocator<T>&, const HotAllocator<U>&) { return true; }

        template<class T, class U>
        bool operator!=(const HotAllocator<T>&, const HotAllocator<U>&) { return false; }

    }

}
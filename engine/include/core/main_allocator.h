#pragma once

#define main_alloc(size) xpe::core::MemoryPoolManager::MainPools->Allocate(size)
#define main_allocT(T, count) static_cast<T*>(main_alloc(sizeof(T) * count))
#define main_construct(T) xpe::core::MemoryPoolManager::MainPools->AllocateConstruct<T>()
#define main_construct_args(T, ...) xpe::core::MemoryPoolManager::MainPools->AllocateConstructArgs<T>(__VA_ARGS__)
#define main_free(addr) xpe::core::MemoryPoolManager::MainPools->Free(addr)
#define main_destruct(T, addr) \
addr->~T();               \
main_free(addr)

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
                return static_cast<T*>(main_alloc(sizeof(T) * count));
            }

            void deallocate(T* element, std::size_t count) noexcept
            {
                main_free(element);
            }
        };

        template<class T, class U>
        bool operator==(const MainAllocator<T>&, const MainAllocator<U>&) { return true; }

        template<class T, class U>
        bool operator!=(const MainAllocator<T>&, const MainAllocator<U>&) { return false; }

    }

}
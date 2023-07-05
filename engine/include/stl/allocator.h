#pragma once

namespace xpe {

    namespace core {

        template<typename T>
        struct Allocator
        {

            typedef T value_type;

            Allocator() = default;

            template<class U>
            constexpr Allocator(const Allocator<U>&) noexcept {}

            [[nodiscard]] T* allocate(std::size_t count)
            {
                return static_cast<T*>(MemoryPoolManager::Allocate(sizeof(T) * count));
            }

            void deallocate(T* element, std::size_t count) noexcept
            {
                MemoryPoolManager::Free(element);
            }
        };

        template<class T, class U>
        bool operator==(const Allocator<T>&, const Allocator<U>&) { return true; }

        template<class T, class U>
        bool operator!=(const Allocator<T>&, const Allocator<U>&) { return false; }

    }

}
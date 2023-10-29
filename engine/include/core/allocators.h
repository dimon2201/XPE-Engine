#pragma once

#define hot_alloc(size) xpe::core::MemoryManager::HotPools->Allocate(size)
#define hot_allocT(T, count) static_cast<T*>(hot_alloc(sizeof(T) * count))
#define hot_construct(T) xpe::core::Memory::HotPools->AllocateConstruct<T>()
#define hot_construct_args(T, ...) xpe::core::MemoryManager::HotPools->AllocateConstructArgs<T>(__VA_ARGS__)
#define hot_free(addr) xpe::core::MemoryManager::HotPools->Free(addr)
#define hot_destruct(T, addr) \
addr->~T();                \
hot_free(addr)

#define main_alloc(size) xpe::core::MemoryManager::MainPools->Allocate(size)
#define main_allocT(T, count) static_cast<T*>(main_alloc(sizeof(T) * count))
#define main_construct(T) xpe::core::Memory::MainPools->AllocateConstruct<T>()
#define main_construct_args(T, ...) xpe::core::MemoryManager::MainPools->AllocateConstructArgs<T>(__VA_ARGS__)
#define main_free(addr) xpe::core::MemoryManager::MainPools->Free(addr)
#define main_destruct(T, addr) \
addr->~T();               \
main_free(addr)

#ifdef DEBUG

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define STACK(size) xpe::core::StackAllocator TOKENPASTE2(s_, __LINE__)(size)
#define STACKT(T, count) xpe::core::StackTypedAllocator<T> TOKENPASTE2(s_, __LINE__)(count)

#define salloc(size) alloca(size); STACK(size)
#define sallocT(T, count) static_cast<T*>(alloca(sizeof(T) * count)); STACKT(T, count)

#define LogStackMemory() xpe::core::StackMemory::Log()

#else

#define salloc(size) alloca(size)
#define sallocT(T, count) static_cast<T*>(alloca(sizeof(T) * count))

#define LogStackMemory()

#endif

namespace xpe {

    namespace core {

        ENGINE_API void* align16_malloc(usize size);
        ENGINE_API void  align16_free(void* address);

        template<typename T>
        class HotAllocator
        {

        public:
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

        template<typename T>
        class MainAllocator
        {

        public:
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

        class ENGINE_API StackMemory final
        {
        public:
            static usize TotalAllocCount;
            static usize MemoryOccupiedBytes;

            static void Log();
        };

        struct ENGINE_API StackAllocator final
        {
            usize Size = 0;

            StackAllocator(usize size);
            ~StackAllocator();
        };

        template<typename T>
        struct StackTypedAllocator final
        {
            usize Size = 0;

            StackTypedAllocator(usize count);
            ~StackTypedAllocator();
        };

        template<typename T>
        StackTypedAllocator<T>::StackTypedAllocator(usize count) : Size(sizeof(T) * count)
        {
            StackMemory::TotalAllocCount += 1;
            StackMemory::MemoryOccupiedBytes += Size;
        }

        template<typename T>
        StackTypedAllocator<T>::~StackTypedAllocator()
        {
            StackMemory::TotalAllocCount -= 1;
            StackMemory::MemoryOccupiedBytes -= Size;
        }

    }

}
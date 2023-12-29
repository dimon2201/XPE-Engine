#pragma once

#define hot_alloc(size) xpe::core::cMemoryManager::HotPools->Allocate(size)
#define hot_allocT(T, count) static_cast<T*>(hot_alloc(sizeof(T) * count))
#define hot_construct(T) xpe::core::cMemoryManager::HotPools->AllocateConstruct<T>()
#define hot_construct_args(T, ...) xpe::core::cMemoryManager::HotPools->AllocateConstructArgs<T>(__VA_ARGS__)
#define hot_free(addr) xpe::core::cMemoryManager::HotPools->Free(addr)
#define hot_destruct(T, addr) \
addr->~T();                \
hot_free(addr)

#define main_alloc(size) xpe::core::cMemoryManager::MainPools->Allocate(size)
#define main_allocT(T, count) static_cast<T*>(main_alloc(sizeof(T) * count))
#define main_construct(T) xpe::core::cMemoryManager::MainPools->AllocateConstruct<T>()
#define main_construct_args(T, ...) xpe::core::cMemoryManager::MainPools->AllocateConstructArgs<T>(__VA_ARGS__)
#define main_free(addr) xpe::core::cMemoryManager::MainPools->Free(addr)
#define main_destruct(T, addr) \
addr->~T();               \
main_free(addr)

#ifdef DEBUG

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define STACK(size) xpe::core::sStackAllocator TOKENPASTE2(s_, __LINE__)(size)
#define STACKT(T, count) xpe::core::sStackTypedAllocator<T> TOKENPASTE2(s_, __LINE__)(count)

#define salloc(size) alloca(size); STACK(size)
#define sallocT(T, count) static_cast<T*>(alloca(sizeof(T) * count)); STACKT(T, count)

#define LogStackMemory() xpe::core::cMemoryStackManager::Log()

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
        class cHotAllocator
        {

        public:
            typedef T value_type;

            cHotAllocator() = default;

            template<class U>
            constexpr cHotAllocator(const cHotAllocator<U>&) noexcept {}

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
        bool operator==(const cHotAllocator<T>&, const cHotAllocator<U>&) { return true; }

        template<class T, class U>
        bool operator!=(const cHotAllocator<T>&, const cHotAllocator<U>&) { return false; }

        template<typename T>
        class cMainAllocator
        {

        public:
            typedef T value_type;

            cMainAllocator() = default;

            template<class U>
            constexpr cMainAllocator(const cMainAllocator<U>&) noexcept {}

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
        bool operator==(const cMainAllocator<T>&, const cMainAllocator<U>&) { return true; }

        template<class T, class U>
        bool operator!=(const cMainAllocator<T>&, const cMainAllocator<U>&) { return false; }

        class ENGINE_API cMemoryStackManager final
        {
        public:
            static usize TotalAllocCount;
            static usize MemoryOccupiedBytes;
            static void Log();
        };

        struct ENGINE_API sStackAllocator final
        {
            usize Size = 0;

            sStackAllocator(usize size);
            ~sStackAllocator();
        };

        template<typename T>
        struct sStackTypedAllocator final
        {
            usize Size = 0;

            sStackTypedAllocator(usize count);
            ~sStackTypedAllocator();
        };

        template<typename T>
        sStackTypedAllocator<T>::sStackTypedAllocator(usize count) : Size(sizeof(T) * count)
        {
            cMemoryStackManager::TotalAllocCount += 1;
            cMemoryStackManager::MemoryOccupiedBytes += Size;
        }

        template<typename T>
        sStackTypedAllocator<T>::~sStackTypedAllocator()
        {
            cMemoryStackManager::TotalAllocCount -= 1;
            cMemoryStackManager::MemoryOccupiedBytes -= Size;
        }

    }

}
#pragma once

#ifdef DEBUG

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define STACK(size) xpe::core::StackAllocator TOKENPASTE2(s_, __LINE__)(size)
#define STACKT(count) xpe::core::StackAllocator<T> TOKENPASTE2(s_, __LINE__)(count)

#define salloc(size) alloca(size); STACK(size)
#define sallocT(T, count) static_cast<T*>(alloca(sizeof(T) * count)); STACKT(count);

#define LogStackMemory() xpe::core::StackMemory::Log()

#else

#define salloc(size) alloca(size);
#define sallocT(T, count) static_cast<T*>(alloca(sizeof(T) * count));

#define LogStackMemory()

#endif

namespace xpe {

    namespace core {

        struct ENGINE_API StackMemory final {
            static usize TotalAllocCount;
            static usize MemoryOccupiedBytes;

            static void Log();
        };

        struct ENGINE_API StackAllocator final {

            usize Size = 0;

            StackAllocator(usize size);
            ~StackAllocator();

        };

        template<typename T>
        struct StackTypedAllocator final {

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
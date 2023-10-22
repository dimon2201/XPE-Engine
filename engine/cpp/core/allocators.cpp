#include <core/allocators.h>

namespace xpe {

    namespace core {

        void* align16_malloc(usize size)
        {

#ifdef WINDOWS
            return _aligned_malloc(size, 16);
#elif  LINUX
            return ::memalign(16, size);
#else
            return main_alloc(size);
#endif

        }

        void align16_free(void* address)
        {

#ifdef WINDOWS
            return _aligned_free(address);
#elif  LINUX
            return ::free(address);
#else
            return main_free(address);
#endif

        }

        usize StackMemory::TotalAllocCount = 0;
        usize StackMemory::MemoryOccupiedBytes = 0;

        void StackMemory::Log() {
            hstringstream ss;

            ss << "\n\n----------- Stack Memory -------------\n";

            ss << "Usage = " << (double) MemoryOccupiedBytes / K_MEMORY_KIB << "KB, "
               << "Allocs = " << TotalAllocCount << "\n";

            ss << "--------------------------------------\n";

            LogMemory(ss.str());
        }

        StackAllocator::StackAllocator(usize size) : Size(size)
        {
            StackMemory::TotalAllocCount += 1;
            StackMemory::MemoryOccupiedBytes += size;
        }

        StackAllocator::~StackAllocator()
        {
            StackMemory::TotalAllocCount -= 1;
            StackMemory::MemoryOccupiedBytes -= Size;
        }

    }

}
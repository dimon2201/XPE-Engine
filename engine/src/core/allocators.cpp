#include <core/allocators.hpp>

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

        usize cMemoryStackManager::TotalAllocCount = 0;
        usize cMemoryStackManager::MemoryOccupiedBytes = 0;

        void cMemoryStackManager::Log() {
            hstringstream ss;

            ss << "\n\n----------- Stack Memory -------------\n";

            ss << "Usage = " << (double) MemoryOccupiedBytes / K_MEMORY_KB << "KB, "
               << "Allocs = " << TotalAllocCount << "\n";

            ss << "--------------------------------------\n";

            LogMemory(ss.str());
        }

        sStackAllocator::sStackAllocator(usize size) : Size(size)
        {
            cMemoryStackManager::TotalAllocCount += 1;
            cMemoryStackManager::MemoryOccupiedBytes += size;
        }

        sStackAllocator::~sStackAllocator()
        {
            cMemoryStackManager::TotalAllocCount -= 1;
            cMemoryStackManager::MemoryOccupiedBytes -= Size;
        }

    }

}
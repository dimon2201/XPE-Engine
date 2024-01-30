#include <allocators.hpp>

namespace xpe
{
    namespace core
    {
        void* align16_alloc(usize size)
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

        sStackAllocator::sStackAllocator(usize size) : Size(size)
        {
            MMemory::StackTotalAllocCount += 1;
            MMemory::StackTotalBytes += size;
        }

        sStackAllocator::~sStackAllocator()
        {
            MMemory::StackTotalAllocCount -= 1;
            MMemory::StackTotalBytes -= Size;
        }
    }
}
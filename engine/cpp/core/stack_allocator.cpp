namespace xpe {

    namespace core {

        usize StackMemory::TotalAllocCount = 0;
        usize StackMemory::MemoryOccupiedBytes = 0;

        void StackMemory::Log() {
            LogInfo(
                    "StackMemory: Using = {}KB, Allocs = {}",
                    (double) MemoryOccupiedBytes / K_MEMORY_KIB,
                    TotalAllocCount
            );
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
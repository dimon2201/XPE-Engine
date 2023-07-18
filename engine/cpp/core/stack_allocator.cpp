namespace xpe {

    namespace core {

        usize StackMemory::TotalAllocCount = 0;
        usize StackMemory::MemoryOccupiedBytes = 0;

        void StackMemory::Log() {
            hstringstream ss;

            ss << "\n----------- Stack Memory -------------\n";

            ss << "Usage = " << (double) MemoryOccupiedBytes / K_MEMORY_KIB << "KB, "
            << "Allocs = " << TotalAllocCount << "\n";

            ss << "--------------------------------------\n";

            LogInfo(ss.str());
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
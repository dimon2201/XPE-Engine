namespace xpe {

    namespace core {

        void MemoryPool::Init(const usize byteSize, const usize allocs, const usize alignment)
        {
            if (alignment == 0) {
                m_Memory = malloc(byteSize);
            } else {
                m_Memory = _aligned_malloc(byteSize, alignment);
            }

            m_ByteSize = byteSize;
            m_LastAddress = m_Memory;
            m_MaxAddress = (void*)(((u64)m_Memory) + ((u64)byteSize));
            m_Allocs.reserve(allocs);
            m_Alignment = alignment;
        }

        void MemoryPool::Release()
        {
            if (m_Alignment == 0) {
                free(m_Memory);
            } else {
                _aligned_free(m_Memory);
            }

            m_Allocs.clear();
        }

        void* MemoryPool::Allocate(const usize size)
        {
#ifdef DEBUG
            m_BytesOccupied += size;
#endif

            for (auto& alloc: m_Allocs)
            {
                if (alloc.FreeFlag == 1u && alloc.AllocByteWidth >= size)
                {
                    alloc.FreeFlag = 255u;
                    alloc.OccupiedByteWidth = size;

                    return alloc.Address;
                }

                if ((alloc.AllocByteWidth - alloc.OccupiedByteWidth) >= size)
                {
                    // New allocation
                    MemoryPoolAllocation newAlloc;
                    newAlloc.FreeFlag = 0u;
                    newAlloc.AllocByteWidth = alloc.AllocByteWidth - alloc.OccupiedByteWidth;
                    newAlloc.OccupiedByteWidth = size;
                    newAlloc.Address = (void*)(((u64)alloc.Address) + ((u64)alloc.OccupiedByteWidth));

                    alloc.AllocByteWidth = alloc.OccupiedByteWidth;

                    m_Allocs.push_back(newAlloc);

                    return newAlloc.Address;
                }
            }

            if (m_LastAddress >= m_MaxAddress) { return nullptr; }

            MemoryPoolAllocation newAlloc;
            newAlloc.FreeFlag = 0u;
            newAlloc.AllocByteWidth = size;
            newAlloc.OccupiedByteWidth = size;
            newAlloc.Address = m_LastAddress;
            m_Allocs.push_back(newAlloc);

            // Update last address
            m_LastAddress = (void*)(((u64)m_LastAddress) + ((u64)size));

            return newAlloc.Address;
        }

        bool MemoryPool::Free(void* address)
        {
            for (auto& alloc: m_Allocs)
            {
                if (alloc.Address == address)
                {

#ifdef DEBUG
                    m_BytesFreed += alloc.OccupiedByteWidth;
                    m_LastFreedBytes = alloc.OccupiedByteWidth;
#endif

                    alloc.FreeFlag = 1u;
                    alloc.OccupiedByteWidth = 0;

                    return true;
                }
            }

            return false;
        }

        MemoryPoolStack::MemoryPoolStack
        (
            const char* usid,
            usize poolCount,
            usize poolByteSize,
            usize poolAllocs,
            const usize alignment
        ) : USID(usid), PoolByteSize(poolByteSize), PoolAllocs(poolAllocs), Alignment(alignment)
        {
            Pools.reserve(poolCount);
            for (u32 i = 0 ; i < poolCount ; i++)
            {
                MemoryPool pool;
                pool.Init(poolByteSize, poolAllocs, alignment);
                Pools.emplace_back(pool);
            }
            TotalBytes = poolCount * poolByteSize;
        }

        MemoryPoolStack::~MemoryPoolStack() {
            for (auto& pool : Pools)
            {
                // todo(CheerWizard): Heap corruption will happen here when we have more than 1 pool here!
                pool.Release();
            }
            Pools.clear();
        }

        void* MemoryPoolStack::Allocate(const usize size) {
#ifdef DEBUG
            TotalAllocCount++;
            TotalBytesOccupied += size;
#endif

            void* newAddress = nullptr;

            for (auto& pool : Pools) {
                newAddress = pool.Allocate(size);
                if (newAddress != nullptr) {
                    return newAddress;
                }
            }

            usize poolSize = size <= PoolByteSize ? PoolByteSize : size;
            TotalBytes += poolSize;

            MemoryPool newPool;
            newPool.Init(poolSize, PoolAllocs, Alignment);
            newAddress = newPool.Allocate(size);

            Pools.emplace_back(newPool);

            return newAddress;
        }

        void MemoryPoolStack::Free(void *address) {
            for (auto& pool : Pools) {
                if (pool.Free(address)) {
#ifdef DEBUG
                    TotalFreeCount++;
                    TotalBytesFreed += pool.GetLastFreedBytes();
#endif
                    return;
                }
            }
        }

        void MemoryPoolStack::LogPools() {
            usize poolCount = Pools.size();

            hstringstream ss;

            ss << "\n\n------------------- " << USID << " -------------------\n";

            usize totalMB = TotalBytes / K_MEMORY_MIB;
            usize totalKB = (TotalBytesOccupied - TotalBytesFreed) / K_MEMORY_KIB;

            ss << "Total Size = " << totalMB << "MB, "
            << "Total Usage = " << totalKB << "KB" << "\n"
            << "Total Allocs = " << TotalAllocCount
            << ", Total Frees = " << TotalFreeCount << "\n";

            for (u32 i = 0 ; i < poolCount ; i++) {
                auto& pool = Pools[i];
                usize sizeMB = pool.GetByteSize() / K_MEMORY_MIB;
                usize usageKB = pool.GetBytesUsage() / K_MEMORY_KIB;

                ss << USID << "Pool-" << i << ": "
                << "Total = " << sizeMB << "MB, "
                << "Usage = " << usageKB << "KB, "
                << "Allocs = " << pool.GetAllocCount() << "\n";
            }

            ss << "---------------------------------------------------------\n";

            LogMemory(ss.str());
        }

        MemoryPoolStack* MemoryManager::MainPools = nullptr;
        MemoryPoolStack* MemoryManager::HotPools = nullptr;

        void MemoryManager::Init() {
            // use by default 15% of TOTAL PHYSICAL RAM for main pre-allocation
            usize mainMemorySize = HardwareManager::Memory.TotalPhysical * 0.15;
            MainPools = new MemoryPoolStack("MainMemory", 1, mainMemorySize, 1000, 0);

            // use by default 1MB for hot memory pre-allocation
            usize hotMemorySize = K_MEMORY_MIB;
            HotPools = new MemoryPoolStack("HotMemory", 1, hotMemorySize, 1000, 0);
        }

        void MemoryManager::Free() {
            delete HotPools;
            delete MainPools;
        }

        void MemoryManager::LogPools() {
            MainPools->LogPools();
            HotPools->LogPools();
        }

    }

}
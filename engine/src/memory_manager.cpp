namespace xpe {

    namespace core {

        void cMemoryPool::Init(const usize byteSize, const usize allocs, const usize alignment)
        {
            if (alignment == 0)
            {
                m_Memory = malloc(byteSize);
            }

            else
            {
                m_Memory = _aligned_malloc(byteSize, alignment);
            }

            m_ByteSize = byteSize;
            m_LastAddress = m_Memory;
            m_MaxAddress = (void*)(((u64)m_Memory) + ((u64)byteSize));
            m_Allocs.reserve(allocs);
            m_Alignment = alignment;
        }

        void cMemoryPool::Release()
        {
            if (m_Alignment == 0)
            {
                free(m_Memory);
            }

            else
            {
                _aligned_free(m_Memory);
            }

            m_Allocs.clear();
        }

        void* cMemoryPool::Allocate(const usize size)
        {
#ifdef K_DEBUG
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
                    sMemoryPoolAllocation newAlloc;
                    newAlloc.FreeFlag = 0u;
                    newAlloc.AllocByteWidth = alloc.AllocByteWidth - alloc.OccupiedByteWidth;
                    newAlloc.OccupiedByteWidth = size;
                    newAlloc.Address = (void*)(((u64)alloc.Address) + ((u64)alloc.OccupiedByteWidth));

                    alloc.AllocByteWidth = alloc.OccupiedByteWidth;

                    m_Allocs.emplace_back(newAlloc);

                    return newAlloc.Address;
                }
            }

            if (m_LastAddress >= m_MaxAddress)
            {
                return nullptr;
            }

            sMemoryPoolAllocation newAlloc;
            newAlloc.FreeFlag = 0u;
            newAlloc.AllocByteWidth = size;
            newAlloc.OccupiedByteWidth = size;
            newAlloc.Address = m_LastAddress;
            m_Allocs.emplace_back(newAlloc);

            // Update last address
            m_LastAddress = (void*)(((u64)m_LastAddress) + ((u64)size));

            return newAlloc.Address;
        }

        bool cMemoryPool::Free(void* address)
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

        cMemoryPoolStack::cMemoryPoolStack
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
                cMemoryPool pool;
                pool.Init(poolByteSize, poolAllocs, alignment);
                Pools.emplace_back(pool);
            }
            TotalBytes = poolCount * poolByteSize;
        }

        cMemoryPoolStack::~cMemoryPoolStack()
        {
            for (auto& pool : Pools)
            {
                // todo(V.A.): Heap corruption will happen here when we have more than 1 pool here!
                pool.Release();
            }
            Pools.clear();
        }

        void* cMemoryPoolStack::Allocate(const usize size)
        {
#ifdef K_DEBUG
            TotalAllocCount++;
            TotalBytesOccupied += size;
#endif
            void* newAddress = nullptr;

            for (auto& pool : Pools)
            {
                newAddress = pool.Allocate(size);
                if (newAddress != nullptr)
                {
                    return newAddress;
                }
            }

            usize poolSize = size <= PoolByteSize ? PoolByteSize : size;
            TotalBytes += poolSize;

            cMemoryPool newPool;
            newPool.Init(poolSize, PoolAllocs, Alignment);
            newAddress = newPool.Allocate(size);

            Pools.emplace_back(newPool);

            return newAddress;
        }

        void cMemoryPoolStack::Free(void *address)
        {
            for (auto& pool : Pools)
            {
                if (pool.Free(address))
                {
#ifdef DEBUG
                    TotalFreeCount++;
                    TotalBytesFreed += pool.GetLastFreedBytes();
#endif
                    return;
                }
            }
        }

        void cMemoryPoolStack::LogPools()
        {
            usize poolCount = Pools.size();

            hstringstream ss;

            ss << "\n\n------------------- " << USID << " -------------------\n";

            usize totalMB = TotalBytes / K_MEMORY_MB;
            usize totalUsageKB = (TotalBytesOccupied - TotalBytesFreed) / K_MEMORY_KB;

            ss << "Total Size = " << totalMB << "MB, "
            << "Total Usage = " << totalUsageKB << "KB" << "\n"
            << "Total Allocs = " << TotalAllocCount
            << ", Total Frees = " << TotalFreeCount << "\n";

            for (u32 i = 0 ; i < poolCount ; i++)
            {
                auto& pool = Pools[i];
                usize sizeMB = pool.GetByteSize() / K_MEMORY_MB;
                usize usageKB = pool.GetBytesUsage() / K_MEMORY_KB;

                ss << USID << "Pool-" << i << ": "
                << "Total = " << sizeMB << "MB, "
                << "Usage = " << usageKB << "KB, "
                << "Allocs = " << pool.GetAllocCount() << "\n";
            }

            ss << "---------------------------------------------------------\n";

            LogMemory(ss.str());
        }

        cMemoryPoolStack* MMemory::MainPools = nullptr;
        cMemoryPoolStack* MMemory::HotPools = nullptr;
        usize MMemory::StackTotalAllocCount = 0;
        usize MMemory::StackTotalBytes = 0;

        void MMemory::Init()
        {
            Profile();

            // use by default 1GB for main memory pre-allocation
            usize mainMemorySize = K_MEMORY_GB;
            MainPools = new cMemoryPoolStack("MainMemory", 1, mainMemorySize, 1000, 0);
            // use by default 1MB for hot memory pre-allocation
            usize hotMemorySize = K_MEMORY_MB;
            HotPools = new cMemoryPoolStack("HotMemory", 1, hotMemorySize, 1000, 0);
        }

        void MMemory::Free()
        {
            Profile();

            delete HotPools;
            delete MainPools;
        }

        void MMemory::LogPools()
        {
            Profile();

            MainPools->LogPools();
            HotPools->LogPools();
        }

        void MMemory::LogStack()
        {
            Profile();

            hstringstream ss;
            ss << "\n\n----------- Stack Memory -------------\n";
            ss << "Usage = " << (double) StackTotalBytes / K_MEMORY_KB << "KB, "
               << "Allocs = " << StackTotalAllocCount << "\n";
            ss << "--------------------------------------\n";
            LogMemory(ss.str());
        }
    }
}
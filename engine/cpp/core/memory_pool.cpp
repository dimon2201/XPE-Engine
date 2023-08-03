#include <json/json.h>

namespace xpe {

    namespace core {

        MemoryPool::MemoryPool(const usize byteSize, const usize allocs)
        {
            m_ByteSize = byteSize;
            m_Memory = malloc(m_ByteSize);
            m_LastAddress = m_Memory;
            m_MaxAddress = (void*)(((u64)m_Memory) + ((u64)byteSize));
            m_Allocs.reserve(allocs);
        }

        MemoryPool::~MemoryPool()
        {
            free(m_Memory);
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

        MemoryPoolStorage::MemoryPoolStorage(const char* usid, usize poolCount, usize poolByteSize, usize poolAllocs)
        : USID(usid), PoolByteSize(poolByteSize), PoolAllocs(poolAllocs)
        {
            Pools.reserve(poolCount);
            for (u32 i = 0 ; i < poolCount ; i++)
            {
                Pools.emplace_back(poolByteSize, poolAllocs);
            }
            TotalBytes = poolCount * poolByteSize;
        }

        MemoryPoolStorage::~MemoryPoolStorage() {
            Pools.clear();
        }

        void* MemoryPoolStorage::Allocate(const usize size) {
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
            Pools.emplace_back(poolSize, PoolAllocs);
            auto *newPool = &Pools[Pools.size() - 1];
            newAddress = newPool->Allocate(size);

            return newAddress;
        }

        void MemoryPoolStorage::Free(void *address) {
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

        void MemoryPoolStorage::LogPools() {
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

        MemoryPoolStorage* MemoryPoolManager::MainPools = nullptr;
        MemoryPoolStorage* MemoryPoolManager::HotPools = nullptr;

        void MemoryPoolManager::Init(const MemoryConfig& config) {
            HotPools = new MemoryPoolStorage("HotMemory", 1, config.HotMemoryKB, config.HotAllocs);
            MainPools = new MemoryPoolStorage("MainMemory", 1, config.MainMemoryMB, config.MainAllocs);
        }

        void MemoryPoolManager::Free() {
            delete HotPools;
            delete MainPools;
        }

        void MemoryPoolManager::LogPools() {
            MainPools->LogPools();
            HotPools->LogPools();
        }

    }

}
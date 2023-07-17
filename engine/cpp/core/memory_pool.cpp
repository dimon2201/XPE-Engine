#include <json/json.h>

namespace xpe {

    namespace core {

        MemoryPool::MemoryPool(const usize byteSize, const usize allocs)
        {
            m_ByteSize = byteSize;
            m_Memory = malloc(m_ByteSize);
            m_LastAddress = m_Memory;
            m_MaxAddress = (void*)(((u64)m_Memory) + ((u64)byteSize));
            m_Allocs.reserve(byteSize / allocs);
        }

        MemoryPool::~MemoryPool()
        {
            free(m_Memory);
            m_BytesUsage = 0;
        }

        void* MemoryPool::Allocate(const usize size)
        {
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

#ifdef DEBUG
            m_BytesUsage += newAlloc.OccupiedByteWidth;
#endif

            return newAlloc.Address;
        }

        bool MemoryPool::Free(void* address)
        {
            for (auto& alloc: m_Allocs)
            {
                if (alloc.Address == address)
                {
#ifdef DEBUG
                    m_BytesUsage -= alloc.OccupiedByteWidth;
                    m_LastFreedBytes = alloc.OccupiedByteWidth;
#endif
                    alloc.FreeFlag = 1u;
                    alloc.OccupiedByteWidth = 0;

                    return true;
                }
            }

            return false;
        }

        MemoryPoolStorage::MemoryPoolStorage(const char* usid, usize poolCount, usize poolByteSize, usize poolAllocByteSize)
        : USID(usid), PoolByteSize(poolByteSize), PoolAllocByteSize(poolAllocByteSize)
        {
            Pools.reserve(poolCount);
            for (u32 i = 0 ; i < poolCount ; i++)
            {
                Pools.emplace_back(poolByteSize, poolAllocByteSize);
            }
        }

        MemoryPoolStorage::~MemoryPoolStorage() {
            Pools.clear();
        }

        void* MemoryPoolStorage::Allocate(const usize size) {
#ifdef DEBUG
            TotalAllocCount++;
            TotalBytesUsage += size;
#endif

            void* newAddress = nullptr;

            for (auto& pool : Pools) {
                newAddress = pool.Allocate(size);
                if (newAddress != nullptr) {
                    return newAddress;
                }
            }

            usize poolSize = size <= PoolByteSize ? PoolByteSize : size;
            Pools.emplace_back(poolSize, PoolAllocByteSize);
            auto *newPool = &Pools[Pools.size() - 1];
            newAddress = newPool->Allocate(size);

            return newAddress;
        }

        void MemoryPoolStorage::Free(void *address) {
            for (auto& pool : Pools) {
                if (pool.Free(address)) {
#ifdef DEBUG
                    TotalFreeCount++;
                    TotalBytesUsage -= pool.GetLastFreedBytes();
#endif
                    return;
                }
            }
        }

        void MemoryPoolStorage::LogPools() {
            usize poolCount = Pools.size();

            LogInfo("{}: Pools = {}, Total Usage = {}MB, Total Allocs = {}, Total Frees = {}",
                    USID,
                    poolCount,
                    (double) TotalBytesUsage / K_MEMORY_MIB,
                    TotalAllocCount,
                    TotalFreeCount
            );

            for (u32 i = 0 ; i < poolCount ; i++) {
                auto& pool = Pools[i];
                LogInfo("{}-{}: Total Size = {}MB, Usage = {}MB, Allocs = {}",
                        USID,
                        i,
                        (double) pool.GetByteSize() / K_MEMORY_MIB,
                        (double) pool.GetBytesUsage() / K_MEMORY_MIB,
                        pool.GetAllocCount()
                );
            }

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

        MemoryConfig MemoryConfigReader::Read(const char *filepath) {
            MemoryConfig memoryConfig;

            std::ifstream file(filepath, std::ios::in | std::ios::binary);

            if (!file.is_open()) {
                std::cerr << "Please specify correct path to memory_config.json file!" << std::endl;
                return memoryConfig;
            }

            file.seekg(0, std::ios::end);
            usize fileByteSize = file.tellg();
            char* str = (char*)malloc(fileByteSize);
            memset(str, 0, fileByteSize);
            file.seekg(0, std::ios::beg);
            file.read(str, fileByteSize);
            file.close();

            Parse(std::string(str), memoryConfig);

            free(str);

            return memoryConfig;
        }

        void MemoryConfigReader::Parse(const std::string &source, MemoryConfig &config) {
            Json::Value root;
            Json::Reader reader;
            reader.parse(source, root);

            config.MainMemoryMB = root["main_memory_mb"].asUInt() * K_MEMORY_MIB;
            config.MainAllocs = root["main_allocs"].asUInt();

            config.HotMemoryKB = root["hot_memory_kb"].asUInt() * K_MEMORY_KIB;
            config.HotAllocs = root["hot_allocs"].asUInt();
        }

    }

}
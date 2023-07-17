#pragma once

#ifdef DEBUG

#define LogMemoryPools() xpe::core::MemoryPoolManager::LogPools()

#else

#define LogMemoryPools()

#endif

namespace xpe
{
    namespace core
    {
        struct MemoryPoolAllocation final
        {
            u32 FreeFlag;
            u32 AllocByteWidth;
            u32 OccupiedByteWidth;
            void* Address;
        };

        class ENGINE_API MemoryPool
        {
            public:
                MemoryPool() = default;

                // byte size - actual size in bytes of malloc
                // allocs - count of allocations that will be reserved for later use
                MemoryPool(const usize byteSize, const usize allocs);

                ~MemoryPool();

                void* Allocate(const usize size);
                bool Free(void* address);

                inline usize GetAllocCount() const {
                    return m_Allocs.size();
                }

                inline usize GetByteSize() const {
                    return m_ByteSize;
                }

                inline usize GetBytesUsage() const {
                    return m_BytesUsage;
                }

                inline usize GetLastFreedBytes() const {
                    return m_LastFreedBytes;
                }

            private:
                usize m_ByteSize;
                void* m_Memory;
                void* m_LastAddress;
                void* m_MaxAddress;
                std::vector<MemoryPoolAllocation> m_Allocs;
                usize m_BytesUsage = 0;
                usize m_LastFreedBytes = 0;
        };

        struct ENGINE_API MemoryPoolStorage final {
            std::vector<MemoryPool> Pools;
            usize PoolByteSize = 0;
            usize PoolAllocByteSize = 0;
            usize TotalAllocCount = 0;
            usize TotalFreeCount = 0;
            usize TotalBytesUsage = 0;
            const char* USID = nullptr;

            MemoryPoolStorage(const char* usid, usize poolCount, usize poolByteSize, usize poolAllocByteSize);

            ~MemoryPoolStorage();

            inline usize GetPoolCount() const {
                return Pools.size();
            }

            inline usize GetAllocCount(u32 index) const {
                return Pools[index].GetAllocCount();
            }

            inline usize GetPoolSize(u32 index) const {
                return Pools[index].GetByteSize();
            }

            void* Allocate(const usize size);
            void Free(void* address);

            template<typename T>
            T* AllocateConstruct();

            template<typename T, typename ... Args>
            T* AllocateConstruct(Args &&... args);

            void LogPools();
        };

        template<typename T>
        T* MemoryPoolStorage::AllocateConstruct() {
            T* address = static_cast<T*>(Allocate(sizeof(T)));
            new (address) T();
            return address;
        }

        template<typename T, typename... Args>
        T* MemoryPoolStorage::AllocateConstruct(Args &&... args) {
            T* address = static_cast<T*>(Allocate(sizeof(T)));
            new (address) T(std::forward<Args>(args)...);
            return address;
        }

        struct ENGINE_API MemoryConfig final {

            usize MainMemoryMB = 1024 * K_MEMORY_MIB;
            usize MainAllocs = 1000;

            usize HotMemoryKB = 1024 * K_MEMORY_KIB;
            usize HotAllocs = 1000;

            static MemoryConfig& Get() {
                static MemoryConfig instance;
                return instance;
            }

        };

        class ENGINE_API MemoryConfigReader final {

        public:
            static MemoryConfig Read(const char* filepath);

        private:
            static void Parse(const std::string& source, MemoryConfig& config);

        };

        class ENGINE_API MemoryPoolManager final {

        public:
            static MemoryPoolStorage* MainPools;
            static MemoryPoolStorage* HotPools;

        public:
            static void Init(const MemoryConfig& config);
            static void Free();
            static void LogPools();

        };

    }
}

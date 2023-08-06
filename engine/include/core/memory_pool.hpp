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
                // byte size - actual size in bytes of malloc
                // allocs - count of allocations that will be reserved for later use
                void Init(const usize byteSize, const usize allocs);
                void Release();

                void* Allocate(const usize size);
                bool Free(void* address);

                inline usize GetAllocCount() const
                {
                    return m_Allocs.size();
                }

                inline usize GetByteSize() const
                {
                    return m_ByteSize;
                }

                inline usize GetBytesUsage() const
                {
                    return m_BytesOccupied - m_BytesFreed;
                }

                inline usize GetLastFreedBytes() const
                {
                    return m_LastFreedBytes;
                }

            private:
                usize m_ByteSize;
                void* m_Memory;
                void* m_LastAddress;
                void* m_MaxAddress;
                std::vector<MemoryPoolAllocation> m_Allocs;
                usize m_BytesOccupied = 0;
                usize m_BytesFreed = 0;
                usize m_LastFreedBytes = 0;
        };

        struct ENGINE_API MemoryPoolStorage final {
            std::vector<MemoryPool> Pools;
            usize PoolByteSize = 0;
            usize PoolAllocs = 0;
            usize TotalAllocCount = 0;
            usize TotalFreeCount = 0;
            usize TotalBytes = 0;
            usize TotalBytesOccupied = 0;
            usize TotalBytesFreed = 0;
            const char* USID = nullptr;

            MemoryPoolStorage(const char* usid, usize poolCount, usize poolByteSize, usize poolAllocs);

            ~MemoryPoolStorage();

            inline usize GetPoolCount() const
            {
                return Pools.size();
            }

            inline usize GetAllocCount(u32 index) const
            {
                return Pools[index].GetAllocCount();
            }

            inline usize GetPoolSize(u32 index) const
            {
                return Pools[index].GetByteSize();
            }

            void* Allocate(const usize size);
            void Free(void* address);

            template<typename T>
            T* AllocateConstruct();

            template<typename T, typename ... Args>
            T* AllocateConstructArgs(Args &&... args);

            void LogPools();
        };

        template<typename T>
        T* MemoryPoolStorage::AllocateConstruct()
        {
            T* address = static_cast<T*>(Allocate(sizeof(T)));
            new (address) T();
            return address;
        }

        template<typename T, typename... Args>
        T* MemoryPoolStorage::AllocateConstructArgs(Args &&... args)
        {
            T* address = static_cast<T*>(Allocate(sizeof(T)));
            new (address) T(std::forward<Args>(args)...);
            return address;
        }

        class ENGINE_API MemoryPoolManager final
        {

        public:
            static MemoryPoolStorage* MainPools;
            static MemoryPoolStorage* HotPools;

        public:
            static void Init();
            static void Free();
            static void FreeMainMemory(void* address);
            static void FreeHotMemory(void* address);
            static void LogPools();

        };

    }
}

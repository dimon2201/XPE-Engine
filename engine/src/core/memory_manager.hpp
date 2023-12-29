#pragma once

#ifdef DEBUG

#define LogMemoryPools() xpe::core::cMemoryManager::LogPools()

#else

#define LogMemoryPools()

#endif

namespace xpe
{
    namespace core
    {
        struct sMemoryPoolAllocation final
        {
            u8 FreeFlag = 0;
            u32 AllocByteWidth = 0;
            u32 OccupiedByteWidth = 0;
            void* Address = nullptr;
        };

        class ENGINE_API cMemoryPool
        {
            public:
                // byte size - actual size in bytes of malloc
                // allocs - count of allocations that will be reserved for later use
                void Init(const usize byteSize, const usize allocs, const usize alignment);
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
                std::vector<sMemoryPoolAllocation> m_Allocs;
                usize m_BytesOccupied = 0;
                usize m_BytesFreed = 0;
                usize m_LastFreedBytes = 0;
                usize m_Alignment = 0;
        };

        class ENGINE_API cMemoryPoolStack final {

        public:
            std::vector<cMemoryPool> Pools;
            usize PoolByteSize = 0;
            usize PoolAllocs = 0;
            usize TotalAllocCount = 0;
            usize TotalFreeCount = 0;
            usize TotalBytes = 0;
            usize TotalBytesOccupied = 0;
            usize TotalBytesFreed = 0;
            usize Alignment = 0;
            const char* USID = nullptr;

            cMemoryPoolStack(const char* usid, usize poolCount, usize poolByteSize, usize poolAllocs, const usize alignment);

            ~cMemoryPoolStack();

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
        T* cMemoryPoolStack::AllocateConstruct()
        {
            T* address = static_cast<T*>(Allocate(sizeof(T)));
            ::new (address) T();
            return address;
        }

        template<typename T, typename... Args>
        T* cMemoryPoolStack::AllocateConstructArgs(Args &&... args)
        {
            T* address = static_cast<T*>(Allocate(sizeof(T)));
            ::new (address) T(std::forward<Args>(args)...);
            return address;
        }

        class ENGINE_API cMemoryManager final
        {

        public:
            static cMemoryPoolStack* MainPools;
            static cMemoryPoolStack* HotPools;

            static void Init();
            static void Free();
            static void LogPools();
        };

    }
}
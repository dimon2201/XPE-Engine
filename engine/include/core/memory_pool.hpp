#pragma once

namespace xpe
{
    namespace core
    {
        struct MemoryPoolAllocation
        {
            u32 _freeFlag;
            u32 _allocByteWidth;
            u32 _occupiedByteWidth;
            void* _address;
        };

        class MemoryPool
        {
            public:
                explicit MemoryPool(const usize size);
                ~MemoryPool();

                void* Allocate(const usize size);
                bool Free(void* address);
                u32 GetCount();

            private:
                usize _size;
                void* _memory;
                void* _lastAddress;
                void* _maxAddress;
                std::vector<MemoryPoolAllocation> _allocs;
        };

        class ENGINE_API MemoryPoolManager final {

        public:
            static MemoryPool* CreatePool(const usize size);

            static void* Allocate(const usize size);

            template<typename T>
            static T* Allocate();

            template<typename T>
            static T* AllocateConstruct();

            template<typename T, typename ... Args>
            static T* AllocateConstruct(Args &&... args);

            static void Free(void* address);

            static u32 GetPoolCount();
            static u32 GetAllocCount(u32 index);

        private:
            static std::vector<MemoryPool*> s_memoryPools;
        };

        template<typename T>
        T* MemoryPoolManager::Allocate() {
            return static_cast<T*>(Allocate(sizeof(T)));
        }

        template<typename T>
        T* MemoryPoolManager::AllocateConstruct() {
            T* address = Allocate<T>();
            new (address) T();
            return address;
        }

        template<typename T, typename... Args>
        T* MemoryPoolManager::AllocateConstruct(Args &&... args) {
            T* address = Allocate<T>();
            new (address) T(std::forward<Args>(args)...);
            return address;
        }

    }
}
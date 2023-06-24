#pragma once

#include <vector>
#include <map>
#include <glm.hpp>

#include "types.hpp"
#include "memory_pool.hpp"

namespace xpe
{
    namespace core
    {
        struct ComponentStorage
        {
            ComponentStorage(void* storage) :
                Storage(storage), Count(0)
            {} 

            void* Storage;
            usize Count;
        };

        class ECSManager
        {
            public:
                const usize k_maxComponentCount = 1024;

                ECSManager() {}
                ~ECSManager() {}

                template <typename ComponentType>
                ComponentStorage* GetComponentStorage()
                {
                    ComponentType c = ComponentType();
                    auto it = _storages.find(c.GetType());

                    if (it == _storages.end())
                    {
                        return nullptr;
                    }
                    else
                    {
                        return &it->second;
                    }
                }

                template <typename ComponentType>
                ComponentType* CreateComponent()
                {
                    ComponentType c = ComponentType();
                    ComponentStorage* storage = GetComponentStorage<ComponentType>();

                    if (storage == nullptr)
                    {
                        MemoryPool* memoryPool = GetMemoryPool(0);
                        void* storageMemory = memoryPool->Allocate(sizeof(ComponentType) * k_maxComponentCount);

                        _storages.insert({ c.GetType(), { storageMemory } });
                        
                        storage = &_storages.find(c.GetType())->second;
                    }

                    ComponentType* storageMemory = (ComponentType*)storage->Storage;
                    storageMemory[storage->Count] = ComponentType();
                    storage->Count += 1;

                    return &storageMemory[storage->Count - 1];
                }

            private:
                std::map<uword, ComponentStorage> _storages;
        };

        struct GeometryComponent : public Object
        {
        };
    }
}
#pragma once

#include <vector>
#include <map>
#include <string>
#include <glm.hpp>

#include "types.hpp"
#include "memory_pool.hpp"

namespace xpe
{
    namespace core
    {
        struct cEntity : public Object
        {
            cEntity(const std::string& usid) : USID(usid)
            {}

            std::string USID;
        };

        struct cComponent : public Object
        {
            cComponent(const std::string& usid) : USID(usid)
            {}

            std::string USID;
        };

        struct cTransformComponent : public cComponent
        {
            cTransformComponent(const std::string& usid) : cComponent(usid)
            {}

            glm::vec3 Position;
            glm::vec3 Rotation;
        };

        struct cViewerComponent : public cComponent
        {
            cViewerComponent(const std::string& usid) : cComponent(usid)
            {}

            f32 _Keys[4];
            boolean _LMBFlag;
            glm::mat4 ViewProjection;
        };
        
        struct xComponentStorage
        {
            xComponentStorage(void* storage) :
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

                cEntity* CreateEntity(const std::string& usid)
                {
                    _entities.insert({ usid, { usid } });

                    auto it = _entities.find(usid);
                    return &it->second;
                }

                cEntity* GetEntity(const std::string& usid)
                {
                    auto it = _entities.find(usid);

                    if (it == _entities.end())
                    {
                        return nullptr;
                    }
                    else
                    {
                        return &it->second;
                    }
                }

                void RemoveEntity(const std::string& usid)
                {
                    auto it = _entities.find(usid);

                    if (it != _entities.end())
                    {
                        _entities.erase(it);
                    }
                }

                template <typename ComponentType>
                xComponentStorage* GetComponentStorage()
                {
                    ComponentType c = ComponentType("");
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
                ComponentType* CreateComponent(const std::string& usid)
                {
                    ComponentType c = ComponentType("");
                    xComponentStorage* storage = GetComponentStorage<ComponentType>();

                    if (storage == nullptr)
                    {
                        MemoryPool* memoryPool = GetMemoryPool(0);
                        void* storageMemory = memoryPool->Allocate(sizeof(ComponentType) * k_maxComponentCount);

                        _storages.insert({ c.GetType(), { storageMemory } });
                        
                        storage = &_storages.find(c.GetType())->second;
                    }

                    ComponentType* storageMemory = (ComponentType*)storage->Storage;
                    storageMemory[storage->Count] = ComponentType(usid);

                    _components.insert({ usid, &storageMemory[storage->Count] });

                    storage->Count += 1;

                    return &storageMemory[storage->Count - 1];
                }

                template <typename ComponentType>
                ComponentType* GetComponent(const std::string& usid)
                {
                    auto it = _components.find(usid);

                    if (it == _components.end())
                    {
                        return nullptr;
                    }
                    else
                    {
                        return it->second;
                    }
                }

                template <typename ComponentType>
                ComponentType* RemoveComponent(const std::string& usid)
                {
                    xComponentStorage* storage = GetComponentStorage<ComponentType>();

                    if (storage == nullptr)
                    {
                        return nullptr;
                    }
                    else if (storage->Count > 0)
                    {
                        ComponentType* storageMemory = (ComponentType*)storage->Storage;

                        usize index = 0;
                        while (storageMemory[index]->USID != usid) { index++; }

                        std::string lastUSID = storageMemory[storage->Count - 1]->USID;
                        auto it = _components.find(lastUSID);
                        if (it != _components.end())
                        {
                            _components.erase(it);
                            _components.insert({ lastUSID, &storageMemory[index] })
                        }

                        storageMemory[index] = storageMemory[storage->Count - 1];
                        storage->Count -= 1;
                    }
                }

            private:
                std::map<std::string, cEntity> _entities;
                std::map<uword, xComponentStorage> _storages;
                std::map<std::string, cComponent*> _components;
        };
    }
}
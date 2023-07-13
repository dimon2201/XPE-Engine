#pragma once

namespace xpe
{
    namespace core
    {
        struct ENGINE_API cEntity : public Object
        {
            cEntity(const string& usid) : USID(usid)
            {}

            string USID;
        };

        struct ENGINE_API cComponent : public Object
        {
            cComponent(const string& usid) : USID(usid)
            {}

            string USID;
        };

        struct ENGINE_API cTransformComponent : public cComponent
        {
            cTransformComponent(const string& usid) : cComponent(usid)
            {}

            glm::vec3 Position;
            glm::vec3 Rotation;
            glm::vec3 Scale;
        };

        struct ENGINE_API cRigidBodyComponent : public cComponent
        {
            cRigidBodyComponent(const string& usid) : cComponent(usid)
            {}

            glm::vec3 Position;
            glm::vec3 Rotation;
        };

        struct ENGINE_API cText2dComponent : public cComponent
        {
            cText2dComponent(const string& usid) : cComponent(usid)
            {}
        };

        struct ENGINE_API cText3dComponent : public cComponent
        {
            cText3dComponent(const string& usid) : cComponent(usid)
            {}
        };

        struct ENGINE_API xComponentStorage
        {
            xComponentStorage(void* storage) :
                Storage(storage), Count(0)
            {} 

            void* Storage;
            usize Count;
        };

        class ENGINE_API ECSManager
        {
            public:
                const usize k_maxComponentCount = 1024;

                ECSManager() {}
                ~ECSManager() {}

                cEntity* CreateEntity(const string& usid)
                {
                    _entities.insert({ usid, { usid } });

                    auto it = _entities.find(usid);
                    return &it->second;
                }

                cEntity* GetEntity(const string& usid)
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

                void RemoveEntity(const string& usid)
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
                ComponentType* CreateComponent(const string& usid)
                {
                    ComponentType c = ComponentType("");
                    xComponentStorage* storage = GetComponentStorage<ComponentType>();

                    if (storage == nullptr)
                    {
                        void* storageMemory = MemoryPoolManager::Allocate(sizeof(ComponentType) * k_maxComponentCount);

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
                ComponentType* GetComponent(const string& usid)
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

                        string lastUSID = storageMemory[storage->Count - 1]->USID;
                        auto it = _components.find(lastUSID);
                        if (it != _components.end())
                        {
                            _components.erase(it);
                            _components.insert({ lastUSID, &storageMemory[index] });
                        }

                        storageMemory[index] = storageMemory[storage->Count - 1];
                        storage->Count -= 1;

                        return storageMemory;
                    }

                    else {
                        return nullptr;
                    }
                }

            private:
                map<string, cEntity> _entities;
                map<uword, xComponentStorage> _storages;
                map<string, cComponent*> _components;
        };
    }
}
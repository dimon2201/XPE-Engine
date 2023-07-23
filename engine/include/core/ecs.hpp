#pragma once

namespace xpe
{
    namespace core
    {
        struct ENGINE_API Entity : public Object
        {
            Entity(const string& usid) : USID(usid)
            {}

            string USID;
        };

        struct ENGINE_API Component : public Object
        {
            Component(const string& usid) : USID(usid)
            {}

            string USID;
        };

        struct ENGINE_API TransformComponent : public Component
        {
            TransformComponent(const string& usid) : Component(usid)
            {}

            TransformComponent(const string& usid, u32 index) : Component(usid), Index(index)
            {}

            u32 Index = 0;
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Rotation = { 0, 0, 0 };
            glm::vec3 Scale = { 1, 1, 1 };
        };

        struct ENGINE_API Transform2DComponent : public Component
        {
            Transform2DComponent(const string& usid) : Component(usid)
            {}

            Transform2DComponent(const string& usid, u32 index) : Component(usid), Index(index)
            {}

            u32 Index = 0;
            glm::vec2 Position = { 0, 0 };
            float Rotation = 0;
            glm::vec2 Scale = { 1, 1 };
        };

        struct ENGINE_API RigidBodyComponent : public Component
        {
            RigidBodyComponent(const string& usid) : Component(usid)
            {}

            glm::vec3 Position;
            glm::vec3 Rotation;
        };

        struct ENGINE_API ComponentStorage
        {
            ComponentStorage(void* storage) : Storage(storage), Count(0)
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

                Entity* CreateEntity(const string& usid)
                {
                    m_Entities.insert({usid, {usid } });

                    auto it = m_Entities.find(usid);
                    return &it->second;
                }

                Entity* GetEntity(const string& usid)
                {
                    auto it = m_Entities.find(usid);

                    if (it == m_Entities.end())
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
                    auto it = m_Entities.find(usid);

                    if (it != m_Entities.end())
                    {
                        m_Entities.erase(it);
                    }
                }

                template <typename ComponentType>
                ComponentStorage* GetComponentStorage()
                {
                    ComponentType c = ComponentType("");
                    auto it = m_Storages.find(c.GetType());

                    if (it == m_Storages.end())
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
                    ComponentStorage* storage = GetComponentStorage<ComponentType>();

                    if (storage == nullptr)
                    {
                        void* storageMemory = alloc(sizeof(ComponentType) * k_maxComponentCount);

                        m_Storages.insert({c.GetType(), {storageMemory } });
                        
                        storage = &m_Storages.find(c.GetType())->second;
                    }

                    ComponentType* storageMemory = (ComponentType*)storage->Storage;
                    storageMemory[storage->Count] = ComponentType(usid);

                    m_Components.insert({usid, &storageMemory[storage->Count] });

                    storage->Count += 1;

                    return &storageMemory[storage->Count - 1];
                }

                template <typename ComponentType>
                ComponentType* GetComponent(const string& usid)
                {
                    auto it = m_Components.find(usid);

                    if (it == m_Components.end())
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
                    ComponentStorage* storage = GetComponentStorage<ComponentType>();

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
                        auto it = m_Components.find(lastUSID);
                        if (it != m_Components.end())
                        {
                            m_Components.erase(it);
                            m_Components.insert({lastUSID, &storageMemory[index] });
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
                map<string, Entity> m_Entities;
                map<uword, ComponentStorage> m_Storages;
                map<string, Component*> m_Components;
        };
    }
}
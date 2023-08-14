#pragma once

#include <ecs/component.h>

namespace xpe {

    namespace ecs {

        class Entity;
        class Global;

        typedef usize ComponentType;
        typedef usize GlobalType;

        struct ENGINE_API Scene : public Object, res::JsonObject
        {
            virtual ~Scene();

            void AddEntity(const string& tag, Entity* entity);

            void RemoveEntity(const string& tag);

            void RenameEntity(const string& oldTag, const string& newTag);

            Entity* GetEntity(const string& tag);

            inline unordered_map<string, Entity*>& GetEntities() { return m_Entities; }

            template<typename T, typename... Args>
            T* AddGlobal(Args&&... args);

            template<typename T>
            void RemoveGlobal();

            template<typename T>
            T* GetGlobal();

            inline unordered_map<uword, Global*>& GetGlobals() { return m_Globals; }

            template<typename T>
            void ReserveComponents(usize capacity);

            template<typename T, typename... Args>
            T* AddComponent(const string& entityTag, Args&&... args);

            template<typename T>
            void RemoveComponent(const string& entityTag, const string& componentTag);

            template<typename T>
            void RenameComponent(const string& oldTag, const string& newTag);

            template<typename T>
            T* GetComponent(const string& entityTag, const string& componentTag);

            template<typename T>
            ComponentStorage& GetComponents();

            template<typename T>
            void EachComponent(const std::function<void(T*)>& iterateFunction);

            template<typename T>
            usize GetComponentsCount();

        protected:
            template<typename T>
            inline ComponentType GetComponentType() { return typeid(T).hash_code(); }

            template<typename T>
            inline GlobalType GetGlobalType() { return typeid(T).hash_code(); }

            template<typename T>
            void InvalidateComponentAddresses();

            void InvalidateComponentAddresses(ComponentType componentType, usize componentSize);

            unordered_map<string, Entity*> m_Entities;
            unordered_map<ComponentType, ComponentStorage> m_ComponentStorages;
            unordered_map<string, unordered_map<ComponentType, unordered_map<string, Component*>>> m_ComponentAddresses;
            unordered_map<GlobalType, Global*> m_Globals;
        };

        template<typename T>
        void Scene::ReserveComponents(usize capacity)
        {
            ComponentType type = GetComponentType<T>();
            m_ComponentStorages[type] = {};
            m_ComponentStorages[type].Reserve<T>(capacity);
        }

        template<typename T, typename... Args>
        T* Scene::AddComponent(const string& entityTag, Args&&... args)
        {
            ComponentType type = GetComponentType<T>();
            T* component;

            // reallocate more memory if capacity exceeds
            auto& componentStorage = m_ComponentStorages[type];
            if (!componentStorage.HasCapacity())
            {
                componentStorage.Reserve<T>(componentStorage.GetSize<T>() * 2 + 1);
                // because component storage memory changed, we need to invalidate all addresses that use it.
                // it may be expensive operation when scene has a lot of entities,
                // but it depends only on component storage capacity and reserve weight variables.
                InvalidateComponentAddresses<T>();
            }
            component = componentStorage.Add<T>(std::forward<Args>(args)...);
            m_ComponentAddresses[entityTag][type][component->GetTag()] = component;

            return component;
        }

        template<typename T>
        void Scene::RemoveComponent(const string& entityTag, const string& componentTag)
        {
            ComponentType type = GetComponentType<T>();
            Component*& component = m_ComponentAddresses[entityTag][type][componentTag];

            if (component == nullptr)
            {
                LogWarning("Component for entity {0} does not exist", entityTag);
                return;
            }

            // remove component from storage
            m_ComponentStorages[type].Erase<T>(entityTag);
            component = nullptr;
        }

        template<typename T>
        void Scene::RenameComponent(const string &oldTag, const string &newTag)
        {
            m_ComponentStorages[GetComponentType<T>()].RenameComponent(oldTag, newTag);

            u64 oldId = Hash(oldTag);
            for (auto& address : m_ComponentAddresses)
            {
                for (auto& entityComponent : address.second)
                {
                    for (auto& component : entityComponent.second)
                    {
                        u64 cid = Hash(component.first);
                        if (oldId == cid)
                        {
                            auto& components = m_ComponentAddresses[address.first][entityComponent.first];
                            components.insert({ newTag, components[component.first] });
                            components.erase(oldTag);
                        }
                    }
                }
            }
        }

        template<typename T>
        T* Scene::GetComponent(const string& entityTag, const string& componentTag)
        {
            return (T*) m_ComponentAddresses[entityTag][GetComponentType<T>()][componentTag];
        }

        template<typename T>
        ComponentStorage& Scene::GetComponents()
        {
            return m_ComponentStorages[GetComponentType<T>()];
        }

        template<typename T>
        void Scene::EachComponent(const std::function<void(T*)>& iterateFunction)
        {
            m_ComponentStorages[GetComponentType<T>()].ForEach<T>(iterateFunction);
        }

        template<typename T>
        void Scene::InvalidateComponentAddresses()
        {
            InvalidateComponentAddresses(GetComponentType<T>(), sizeof(T));
        }

        template<typename T>
        usize Scene::GetComponentsCount()
        {
            return m_ComponentStorages[GetComponentType<T>()].GetSize();
        }

        template<typename T>
        void Scene::RemoveGlobal()
        {
            auto it = m_Globals.find(GetGlobalType<T>());
            if (it != m_Globals.end()) {
                m_Globals.erase(it);
                delete it.operator->();
            }
        }

        template<typename T>
        T* Scene::GetGlobal()
        {
            return static_cast<T*>(m_Globals[GetGlobalType<T>()]);
        }

    }

}
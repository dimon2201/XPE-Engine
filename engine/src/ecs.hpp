#pragma once

#include <entt/entity/registry.hpp>

namespace physx
{
    struct PxScene;
}

namespace xpe::ecs
{
    struct sSkybox;
}

namespace xpe
{
    namespace ecs
    {
        typedef entt::entity EntityID;
        constexpr EntityID EntityNull = entt::null;
        typedef usize ComponentID;
        using cRegistry = entt::basic_registry<EntityID, cMainAllocator<EntityID>>;
        template<typename T>
        using cStorage = entt::basic_storage<T, EntityID, cMainAllocator<EntityID>>;

        class ENGINE_API cScene : public cObject, public cJson, public cXml
        {

        public:
            sSkybox* Skybox = nullptr;
            physx::PxScene* PhysicsScene;

            cScene(physx::PxScene* physicsScene);

            inline void SetTag(const string& tag) { m_Tag = tag; }

            [[nodiscard]] inline const string& GetTag() const { return m_Tag; }

            EntityID CreateEntity();

            void RemoveEntity(EntityID entityId);

            bool IsValidEntity(EntityID entityId);

            void ClearEntities();

            inline const vector<EntityID>& GetEntities() const { return m_Entities; }

            template<typename T, typename... Args>
            T& AddComponent(EntityID entityId, Args&&... args);

            template<typename T>
            void RemoveComponent(EntityID entityId);

            template<typename T>
            T& GetComponent(EntityID entityId);

            template<typename... T>
            bool HasAnyComponent(EntityID entityId);

            template<typename... T>
            bool HasAllComponent(EntityID entityId);

            template<typename... T>
            void ClearComponents();

            template<typename... T>
            auto GetComponents();

            unordered_map<ComponentID, cJson*>& GetJsons(EntityID entityId);
            unordered_map<ComponentID, cXml*>&  GetXmls(EntityID entityId);

            void ToJson(json &root) override;
            void FromJson(json &root) override;
            xml ToXml(xml &root) override;
            xml FromXml(xml &root) override;

        private:
            string m_Tag;
            cRegistry m_Registry;
            unordered_map<EntityID, unordered_map<ComponentID, cJson*>> m_Jsons;
            unordered_map<EntityID, unordered_map<ComponentID, cXml*>>  m_Xmls;
            vector<EntityID> m_Entities; // used only for editor
        };

        template<typename T, typename... Args>
        T& cScene::AddComponent(EntityID entityId, Args&&... args)
        {
            T* component;

            if constexpr(sizeof...(Args) != 0u)
            {
                component = &m_Registry.emplace_or_replace<T>(entityId, std::forward<Args>(args)...);
            }
            else
            {
                component = &m_Registry.emplace_or_replace<T>(entityId, T());
            }

            if constexpr (std::is_base_of<cJson, T>::value)
            {
                m_Jsons[entityId][typeid(T).hash_code()] = component;
            }
            else if constexpr (std::is_base_of<cXml, T>::value)
            {
                m_Xmls[entityId][typeid(T).hash_code()] = component;
            }

            return *component;
        }

        template<typename T>
        void cScene::RemoveComponent(EntityID entityId)
        {
            if (std::is_base_of<cJson, T>::value)
            {
                m_Jsons[entityId].erase(typeid(T).hash_code());
            }
            else if (std::is_base_of<cXml, T>::value)
            {
                m_Xmls[entityId].erase(typeid(T).hash_code());
            }

            if (m_Registry.any_of<T>(entityId))
            {
                m_Registry.erase<T>(entityId);
            }
        }

        template<typename T>
        T& cScene::GetComponent(EntityID entityId)
        {
            return m_Registry.get<T>(entityId);
        }

        template<typename... T>
        bool cScene::HasAnyComponent(EntityID entityId)
        {
            return m_Registry.any_of<T...>(entityId);
        }

        template<typename... T>
        bool cScene::HasAllComponent(EntityID entityId)
        {
            return m_Registry.all_of<T...>(entityId);
        }

        template<typename... T>
        void cScene::ClearComponents()
        {
            m_Registry.clear<T...>();
            m_Jsons.clear();
            m_Xmls.clear();
        }

        template<typename... T>
        auto cScene::GetComponents()
        {
            return m_Registry.view<T...>();
        }

        class ENGINE_API cEntity : public cJson
        {

        public:
            cEntity() = default;
            cEntity(const string& tag, cScene* scene);
            cEntity(EntityID id, cScene* scene) : m_ID(id), m_Scene(scene) {}

            [[nodiscard]] inline const EntityID GetID() const { return m_ID; }

            inline bool IsValid() const { return m_ID != EntityNull; }

            inline void SetScene(cScene* scene) { m_Scene = scene; }

            [[nodiscard]] inline const cScene* GetScene() const { return m_Scene; }

            void SetTag(const string& tag);

            string& GetTag();

            template<typename T, typename... Args>
            T& Add(Args&&... args);

            template<typename T>
            T& Get();

            template<typename T>
            void Remove();

            template<typename... T>
            bool HasAny();

            template<typename... T>
            bool HasAll();

            void ToJson(json &root) override;
            void FromJson(json &root) override;

            inline bool operator ==(const cEntity& other) const { return m_ID == other.GetID(); }

        protected:
            EntityID m_ID = EntityNull;
            cScene* m_Scene = nullptr;
        };

        template<typename T>
        T& cEntity::Get()
        {
            return m_Scene->GetComponent<T>(m_ID);
        }

        template<typename T, typename... Args>
        T& cEntity::Add(Args&&... args)
        {
            return m_Scene->AddComponent<T>(m_ID, std::forward<Args>(args)...);
        }

        template<typename T>
        void cEntity::Remove()
        {
            m_Scene->RemoveComponent<T>(m_ID);
        }

        template<typename... T>
        bool cEntity::HasAny()
        {
            return m_Scene->HasAnyComponent<T...>(m_ID);
        }

        template<typename... T>
        bool cEntity::HasAll()
        {
            return m_Scene->HasAllComponent<T...>(m_ID);
        }
    }
}
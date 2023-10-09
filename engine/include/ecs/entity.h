#pragma once

#include <ecs/scene.h>

namespace xpe {

    namespace ecs {

        struct ENGINE_API Entity : public Object, res::JsonObject
        {
            math::Transform Transform;
            ecs::Entity* Parent = nullptr;
            vector<ecs::Entity*> Children;

            JsonClass(
                Entity,
                m_Tag,
                Transform
            )

            Entity(const string& tag, Scene* scene);

            ~Entity();

            inline void SetScene(Scene* scene) { m_Scene = scene; }

            [[nodiscard]] inline const Scene* GetScene() const { return m_Scene; }

            inline void Rename(const string& tag)
            {
                m_Scene->RenameEntity(m_Tag, tag);
            }

            template<typename T>
            T* GetComponent(const string& componentTag);

            template<typename T>
            T* GetComponent();

            template<typename T, typename... Args>
            T* AddComponent(Args &&... args);

            template<typename T>
            void RemoveComponent(const string& componentTag);

            template<typename T>
            void RenameComponent(const string& oldTag, const string& newTag);

            void RemoveComponents();

            template<typename T>
            bool ValidComponent(const string& componentTag);

            template<typename T>
            bool InvalidComponent(const string& componentTag);

            inline bool operator ==(const Entity& other) const
            {
                return m_Tag == other.GetTag();
            }

        protected:
            Scene* m_Scene = nullptr;
        };

        template<typename T>
        T* Entity::GetComponent(const string& componentTag)
        {
            return m_Scene->GetComponent<T>(m_Tag, componentTag);
        }

        template<typename T>
        T* Entity::GetComponent()
        {
            return m_Scene->GetComponent<T>(m_Tag, m_Tag);
        }

        template<typename T, typename... Args>
        T* Entity::AddComponent(Args &&... args)
        {
            T* newComponent = m_Scene->AddComponent<T>(m_Tag, std::forward<Args>(args)...);
            newComponent->Entity = this;
            return newComponent;
        }

        template<typename T>
        void Entity::RemoveComponent(const string& componentTag)
        {
            m_Scene->RemoveComponent<T>(m_Tag, componentTag);
        }

        template<typename T>
        void Entity::RenameComponent(const string &oldTag, const string &newTag)
        {
            m_Scene->RenameComponent<T>(oldTag, newTag);
        }

        template<typename T>
        bool Entity::ValidComponent(const string& componentTag)
        {
            return GetComponent<T>(componentTag) != nullptr;
        }

        template<typename T>
        bool Entity::InvalidComponent(const string& componentTag)
        {
            return GetComponent<T>(componentTag) == nullptr;
        }

    }

}
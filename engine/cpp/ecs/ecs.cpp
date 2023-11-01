#include <ecs/components.hpp>

namespace xpe {

    namespace ecs {

        sComponent* sComponentStorage::GetAddress(usize componentSize, cEntity* entity)
        {
            usize size = m_Components.size();
            usize step = componentSize;

            for (usize i = 0 ; i < size ; i += step)
            {
                auto* component = reinterpret_cast<sComponent*>(&m_Components[i]);
                if (component->Entity == entity) {
                    return component;
                }
            }

            return nullptr;
        }

        void sComponentStorage::Clear()
        {
            m_Components.clear();
        }

        cEntity::cEntity(const string& tag, cScene* scene) : m_Scene(scene)
        {
            m_Tag = tag;
            if (m_Scene != nullptr) {
                m_Scene->AddEntity(tag, this);
            }
        }

        cEntity::~cEntity()
        {
            if (m_Scene != nullptr) {
                m_Scene->RemoveEntity(m_Tag);
                m_Scene->RemoveComponents(this);
            }
        }

        void cEntity::RemoveAll()
        {
            m_Scene->RemoveComponents(this);
        }

        void cScene::AddEntity(const string& tag, cEntity* entity)
        {
            m_Entities.insert({ tag, entity });
        }

        void cScene::RemoveEntity(const string& tag)
        {
            m_Entities.erase(tag);
        }

        void cScene::RenameEntity(const string &oldTag, const string& newTag)
        {
            m_Entities.insert({ newTag, m_Entities[oldTag] });
            m_Entities.erase(oldTag);
        }

        cEntity* cScene::GetEntity(const string &tag)
        {
            auto it = m_Entities.find(tag);

            if (it != m_Entities.end()) {
                return it->second;
            }

            return nullptr;
        }

        cScene::~cScene()
        {
            m_Entities.clear();

            m_ComponentAddresses.clear();

            m_ComponentStorages.clear();

            for (auto& global : m_Globals)
            {
                delete global.second;
            }
            m_Globals.clear();
        }

        void cScene::InvalidateComponentAddresses(ComponentType type, usize componentSize)
        {
            for (const auto& entity : m_Entities)
            {
                m_ComponentAddresses[entity.second][type] = m_ComponentStorages[type].GetAddress(componentSize, entity.second);
            }
        }

        void cScene::RemoveComponents(cEntity* entity)
        {
            m_ComponentAddresses.erase(entity);
        }

        void cScene::ToJson(json &root)
        {
        }

        void cScene::FromJson(json &root)
        {
        }

        void cEntity::SetPosition(const glm::vec3& position)
        {
            m_Transform.Position = position;

            sCPhysicsActor* actor = Get<sCPhysicsActor>();
            if (actor != nullptr)
            {
                physics::cPhysicsManager::SetActorPose(actor, &m_Transform);
            }
        }

        void cEntity::SetRotation(const glm::vec3& rotation)
        {
            m_Transform.Rotation = rotation;
        }

        void cEntity::SetScale(const glm::vec3& scale)
        {
            m_Transform.Scale = scale;
        }

    }

}
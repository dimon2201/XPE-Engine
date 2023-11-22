#include <ecs/components.hpp>
#include <ecs/ecs.h>


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

        void cEntity::SetTransform(const sTransform &transform)
        {
            m_Transform = transform;

            sCPhysicsActor* actor = Get<sCPhysicsActor>();
            if (actor != nullptr)
            {
                physics::cPhysicsManager::SetActorPose(actor, &m_Transform);
            }

            SetPosition(transform.Position);
            SetRotation(transform.Rotation);
            SetScale(transform.Scale);
        }

        void cEntity::UpdateXmlChildren()
        {
            XmlChildren.clear();
            for (auto* child : Children) {
                XmlChildren.emplace_back(child);
            }
        }

        void cEntity::SetPosition(const glm::vec3 &position)
        {
            glm::vec3 diff = position - m_Transform.Position;
            m_Transform.Position = position;
            for (auto* child : Children) {
                child->Move(diff);
            }
        }

        void cEntity::SetRotation(const glm::vec3 &rotation)
        {
            glm::vec3 diff = rotation - m_Transform.Rotation;
            m_Transform.Rotation = rotation;
            for (auto* child : Children) {
                child->Rotate(diff);
            }
        }

        void cEntity::SetScale(const glm::vec3 &scale)
        {
            glm::vec3 diff = scale - m_Transform.Scale;
            m_Transform.Scale = scale;
            for (auto* child : Children) {
                child->Scale(diff);
            }
        }

        void cEntity::Move(const glm::vec3 &diff)
        {
            m_Transform.Position += diff;
            for (auto* child : Children) {
                child->Move(diff);
            }
        }

        void cEntity::Rotate(const glm::vec3 &diff)
        {
            m_Transform.Rotation += diff;
            for (auto* child : Children) {
                child->Rotate(diff);
            }
        }

        void cEntity::Scale(const glm::vec3 &diff)
        {
            m_Transform.Scale += diff;
            for (auto* child : Children) {
                child->Scale(diff);
            }
        }

        void cEntity::SetVisible(bool visible)
        {
            m_Visible = visible;
            for (auto* child : Children) {
                child->SetVisible(visible);
            }
        }

        void cEntity::SetSpace(eSpace space)
        {
            m_Space = space;
            for (auto* child : Children) {
                child->SetSpace(space);
            }
        }

        xml cEntity::ToXml(xml &root)
        {
            cXml* component = HasAs<cXml>();
            if (component != nullptr) {
                return component->ToXml(root);
            }
            return root;
        }

        xml cEntity::FromXml(xml &root)
        {
            cXml* component = HasAs<cXml>();
            if (component != nullptr) {
                return component->FromXml(root);
            }
            return root;
        }

    }

}
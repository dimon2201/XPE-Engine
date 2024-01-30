#include <ecs.hpp>
#include <components.hpp>

namespace xpe
{
    namespace ecs
    {
        cScene::cScene(physx::PxScene* physicsScene) : PhysicsScene(physicsScene)
        {
            m_Registry.create();
        }

        EntityID cScene::CreateEntity()
        {
            EntityID entityId = m_Registry.create();
            m_Entities.emplace_back(entityId);
            return entityId;
        }

        void cScene::RemoveEntity(EntityID entityId)
        {
            m_Registry.destroy(entityId);
            m_Entities.erase(std::find(m_Entities.begin(), m_Entities.end(), entityId));
        }

        bool cScene::IsValidEntity(EntityID entityId)
        {
            return m_Registry.valid(entityId);
        }

        void cScene::ClearEntities()
        {
            m_Registry.clear();
            m_Entities.clear();
        }

        unordered_map<ComponentID, cJson*>& cScene::GetJsons(EntityID entityId)
        {
            return m_Jsons[entityId];
        }

        unordered_map<ComponentID, cXml*>& cScene::GetXmls(EntityID entityId)
        {
            return m_Xmls[entityId];
        }

        void cScene::ToJson(json &root)
        {
            to_json(root, m_Tag);
            for (auto& jsonEntity : m_Jsons)
            {
                for (auto& jsonComponent : jsonEntity.second)
                {
                    jsonComponent.second->ToJson(root);
                }
            }
        }

        void cScene::FromJson(json &root)
        {
            from_json(root, m_Tag);
            for (auto& jsonEntity : m_Jsons)
            {
                for (auto& jsonComponent : jsonEntity.second)
                {
                    jsonComponent.second->FromJson(root);
                }
            }
        }

        xml cScene::ToXml(xml &root)
        {
            SetXmlValue(root, "Tag", m_Tag);
            for (auto& xmlEntity : m_Xmls)
            {
                for (auto& xmlComponent : xmlEntity.second)
                {
                    return xmlComponent.second->ToXml(root);
                }
            }
            return root;
        }

        xml cScene::FromXml(xml &root)
        {
            GetXmlValue(root, "Tag", m_Tag);
            for (auto& xmlEntity : m_Xmls)
            {
                for (auto& xmlComponent : xmlEntity.second)
                {
                    return xmlComponent.second->FromXml(root);
                }
            }
            return root;
        }

        cEntity::cEntity(const string& tag, cScene* scene) : m_Scene(scene)
        {
            if (m_Scene != nullptr)
            {
                m_ID = m_Scene->CreateEntity();
                Add<CTag>(tag);
                Add<CTransform>();
                Add<CVisible>();
            }
        }

        void cEntity::SetTag(const string &tag)
        {
            Add<CTag>(tag);
        }

        string &cEntity::GetTag()
        {
            return Get<CTag>().Tag;
        }

        void cEntity::ToJson(json &root)
        {
            auto& jsons = m_Scene->GetJsons(m_ID);
            for (auto& _json : jsons)
            {
                _json.second->ToJson(root);
            }
        }

        void cEntity::FromJson(json &root)
        {
            auto& jsons = m_Scene->GetJsons(m_ID);
            for (auto& _json : jsons)
            {
                _json.second->FromJson(root);
            }
        }
    }
}
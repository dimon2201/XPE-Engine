#include <ecs/components.hpp>
#include <ecs/ecs.hpp>

namespace xpe {

    namespace ecs {

        EntityID cScene::CreateEntity()
        {
            return m_Registry.create();
        }

        void cScene::RemoveEntity(EntityID entityId)
        {
            m_Registry.destroy(entityId);
        }

        bool cScene::IsValidEntity(EntityID entityId)
        {
            return m_Registry.valid(entityId);
        }

        void cScene::ClearEntities()
        {
            m_Registry.clear();
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
            if (m_Scene != nullptr) {
                m_ID = m_Scene->CreateEntity();
                Add<CTag>(tag);
                Add<CTransform>();
                Add<CVisible>();
            }
        }

        void cEntity::UpdateXmlChildren()
        {
            XmlChildren.clear();
            for (auto* child : Children) {
                XmlChildren.emplace_back(child);
            }
        }

        void cEntity::SetTag(const string &tag)
        {
            Add<CTag>(tag);
        }

        void cEntity::SetVisible(bool visible)
        {
            if (visible) {
                Add<CVisible>();
            }
            else {
                Remove<CVisible>();
            }

            for (auto* child : Children) {
                child->SetVisible(visible);
            }
        }

        void cEntity::SetSpace2D()
        {
            Add<CSpace2D>();
            for (auto* child : Children) {
                child->SetSpace2D();
            }
        }

        void cEntity::SetSpace3D()
        {
            Add<CSpace3D>();
            for (auto* child : Children) {
                child->SetSpace3D();
            }
        }

        void cEntity::SetTransparent(bool transparent)
        {
            if (transparent) {
                Add<CTransparent>();
            }
            else {
                Remove<CTransparent>();
            }
        }

        void cEntity::SetOpaque(bool opaque)
        {
            if (opaque) {
                Add<COpaque>();
            }
            else {
                Remove<COpaque>();
            }
        }

        const string &cEntity::GetTag()
        {
            return Get<CTag>().Tag;
        }

        sTransform& cEntity::GetTransform()
        {
            return Get<CTransform>();
        }

        glm::vec3& cEntity::GetPosition()
        {
            return Get<CTransform>().Position;
        }

        glm::vec3 &cEntity::GetRotation()
        {
            return Get<CTransform>().Rotation;
        }

        glm::vec3 &cEntity::GetScale()
        {
            return Get<CTransform>().Scale;
        }

        bool cEntity::IsVisible()
        {
            return HasAny<CVisible>();
        }

        bool cEntity::IsTransparent()
        {
            return HasAny<CTransparent>();
        }

        bool cEntity::IsOpaque()
        {
            return HasAny<COpaque>();
        }

        xml cEntity::ToXml(xml &root)
        {
            auto& xmls = m_Scene->GetXmls(m_ID);
            xml newRoot;
            for (auto& _xml : xmls)
            {
                newRoot = _xml.second->ToXml(root);
            }
            return newRoot;
        }

        xml cEntity::FromXml(xml &root)
        {
            auto& xmls = m_Scene->GetXmls(m_ID);
            xml newRoot;
            for (auto& _xml : xmls)
            {
                newRoot = _xml.second->FromXml(root);
            }
            return newRoot;
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

        void cEntity::SetTransformImpl(const sTransform& _transform)
        {
            auto& transform = Add<CTransform>(_transform);

            if (HasAny<CPhysicsActor>())
            {
                physics::cPhysicsManager::SetActorPose(Get<CPhysicsActor>(), transform);
            }

            SetPosition(transform.Position);
            SetRotation(transform.Rotation);
            SetScale(transform.Scale);
        }

        void cEntity::SetPositionImpl(const glm::vec3 &position)
        {
            auto& transform = Get<CTransform>();
            glm::vec3 diff = position - transform.Position;
            transform.Position = position;
            for (auto* child : Children) {
                child->Move(diff);
            }
        }

        void cEntity::SetRotationImpl(const glm::vec3 &rotation)
        {
            auto& transform = Get<CTransform>();
            glm::vec3 diff = rotation - transform.Rotation;
            transform.Rotation = rotation;
            for (auto* child : Children) {
                child->Rotate(diff);
            }
        }

        void cEntity::SetScaleImpl(const glm::vec3 &scale)
        {
            auto& transform = Get<CTransform>();
            glm::vec3 diff = scale - transform.Scale;
            transform.Scale = scale;
            for (auto* child : Children) {
                child->Scale(diff);
            }
        }

        void cEntity::MoveImpl(const glm::vec3 &diff)
        {
            auto& transform = Get<CTransform>();
            transform.Position += diff;
            for (auto* child : Children) {
                child->Move(diff);
            }
        }

        void cEntity::RotateImpl(const glm::vec3 &diff)
        {
            auto& transform = Get<CTransform>();
            transform.Rotation += diff;
            for (auto* child : Children) {
                child->Rotate(diff);
            }
        }

        void cEntity::ScaleImpl(const glm::vec3 &diff)
        {
            auto& transform = Get<CTransform>();
            transform.Scale += diff;
            for (auto* child : Children) {
                child->Scale(diff);
            }
        }

    }

}
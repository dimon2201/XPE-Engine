namespace xpe {

    namespace ecs {

        Component* ComponentStorage::GetAddress(usize componentSize, const string& tag)
        {
            u64 id = Hash(tag);
            usize size = m_Components.size();
            usize step = componentSize;

            for (usize i = 0 ; i < size ; i += step)
            {
                auto* component = reinterpret_cast<Component*>(&m_Components[i]);
                u64 cid = Hash(component->GetTag());
                if (cid == id) {
                    return component;
                }
            }

            return nullptr;
        }

        void ComponentStorage::Clear()
        {
            m_Components.clear();
        }

        Entity::Entity(const string& tag, Scene* scene) : m_Scene(scene)
        {
            m_Tag = tag;
            if (m_Scene != nullptr) {
                m_Scene->AddEntity(tag, this);
            }
        }

        Entity::~Entity()
        {
            if (m_Scene != nullptr) {
                m_Scene->RemoveEntity(m_Tag);
                m_Scene->RemoveComponents(m_Tag);
            }
        }

        void Entity::RemoveComponents()
        {
            m_Scene->RemoveComponents(m_Tag);
        }

        void Scene::AddEntity(const string& tag, Entity* entity)
        {
            m_Entities.insert({ tag, entity });
        }

        void Scene::RemoveEntity(const string& tag)
        {
            m_Entities.erase(tag);
        }

        void Scene::RenameEntity(const string &oldTag, const string& newTag)
        {
            m_Entities.insert({ newTag, m_Entities[oldTag] });
            m_Entities.erase(oldTag);
            m_ComponentAddresses.insert({ newTag, m_ComponentAddresses[oldTag] });
            m_ComponentAddresses.erase(oldTag);
        }

        Entity* Scene::GetEntity(const string &tag)
        {
            auto it = m_Entities.find(tag);

            if (it != m_Entities.end()) {
                return it->second;
            }

            return nullptr;
        }

        Scene::~Scene()
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

        void Scene::InvalidateComponentAddresses(ComponentType type, usize componentSize)
        {
            for (const auto& entity : m_Entities)
            {
                for (auto& entityComponent : m_ComponentAddresses[entity.first][type])
                {
                    entityComponent.second = m_ComponentStorages[type].GetAddress(componentSize, entityComponent.first);
                }
            }
        }

        void Scene::RemoveComponents(const string& entityTag)
        {
            m_ComponentAddresses.erase(entityTag);
        }

    }

}
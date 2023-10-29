namespace xpe {

    namespace ecs {

        Component* ComponentStorage::GetAddress(usize componentSize, Entity* entity)
        {
            usize size = m_Components.size();
            usize step = componentSize;

            for (usize i = 0 ; i < size ; i += step)
            {
                auto* component = reinterpret_cast<Component*>(&m_Components[i]);
                if (component->Entity == entity) {
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
                m_Scene->RemoveComponents(this);
            }
        }

        void Entity::RemoveAll()
        {
            m_Scene->RemoveComponents(this);
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
                m_ComponentAddresses[entity.second][type] = m_ComponentStorages[type].GetAddress(componentSize, entity.second);
            }
        }

        void Scene::RemoveComponents(Entity* entity)
        {
            m_ComponentAddresses.erase(entity);
        }

    }

}
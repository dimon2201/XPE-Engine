#include <ecs/scene.h>
#include <ecs/entity.h>
#include <ecs/global.h>

namespace xpe {

    namespace ecs {

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

        Entity *Scene::GetEntity(const string &tag)
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

            for (auto& address : m_ComponentAddresses)
            {
                for (auto& entityComponent : address.second)
                {
                    for (auto& component : entityComponent.second)
                    {
                        component.second->~Component();
                    }
                }
            }
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

    }

}
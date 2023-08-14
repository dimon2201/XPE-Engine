#pragma once

#include <ecs/entity.h>

namespace xpe {

    namespace ecs {

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
            }
        }

    }

}
#pragma once

#include <rendering/materials/material.h>

namespace xpe {

    namespace ecs {

        struct ENGINE_API MaterialComponent : public Component {

            MaterialComponent(const string& usid) : Component(usid) {}
            MaterialComponent(const string& usid, Material* material) : Component(usid), Material(material) {}

            Material* Material = nullptr;

        };

    }

}
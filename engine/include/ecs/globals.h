#pragma once

#include <ecs/ecs.h>
#include <ecs/components.hpp>

#include <rendering/core/texture.h>
#include <rendering/geometry/geometries.h>

namespace xpe {

    namespace ecs {

        struct ENGINE_API Skybox : public Global
        {
            Ref<render::Geometry> Geometry;
            Ref<render::Texture> Texture;

            JsonClass(
                Skybox,
                m_Tag
            )
        };

    }

}
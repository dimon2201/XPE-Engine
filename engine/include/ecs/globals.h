#pragma once

#include <ecs/ecs.h>
#include <ecs/components.hpp>

#include <rendering/core/texture.h>
#include <rendering/geometry/geometries.h>

namespace xpe {

    namespace ecs {

        struct ENGINE_API Skybox : public Global
        {
            Ref<render::Texture> CubeTexture;
            string FrontResFilepath;
            string BackResFilepath;
            string TopResFilepath;
            string BottomResFilepath;
            string RightResFilepath;
            string LeftResFilepath;
            string AssetFilepath;

            JsonClass(
                Skybox,
                FrontResFilepath,
                BackResFilepath,
                TopResFilepath,
                BottomResFilepath,
                RightResFilepath,
                LeftResFilepath,
                AssetFilepath
            )
        };

    }

}
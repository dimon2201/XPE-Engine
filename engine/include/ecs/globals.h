#pragma once

#include <ecs/global.h>

#include <rendering/core/texture.h>

#include <geometry/geometries.h>

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
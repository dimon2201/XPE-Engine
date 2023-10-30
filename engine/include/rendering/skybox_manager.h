#pragma once

#include <rendering/core/texture.h>
#include <rendering/geometry/geometries.h>

namespace xpe {

    namespace render {

        struct ENGINE_API Skybox final
        {
            render::Geometry Geometry;
            render::Texture* Texture;
        };
        Json(Skybox, Geometry)

        class ENGINE_API SkyboxManager final
        {

        public:
            static void Init();
            static void Free();
            static SkyboxManager& Get();

            render::Skybox Skybox;

        private:
            static SkyboxManager* s_Instance;
        };

    }

}
#pragma once

#include <rendering/core/texture.h>
#include <rendering/geometry/geometries.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sSkybox final
        {
            sGeometry Geometry;
            sTexture* Texture;
        };
        Json(sSkybox, Geometry)

        class ENGINE_API cSkyboxManager final
        {

        public:
            static void Init();
            static void Free();
            static cSkyboxManager& Get();

            sSkybox Skybox;

        private:
            static cSkyboxManager* s_Instance;
        };

    }

}
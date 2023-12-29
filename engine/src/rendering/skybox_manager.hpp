#pragma once

#include <rendering/core/texture.hpp>
#include <rendering/geometry/geometries.hpp>

namespace xpe {

    namespace render {

        struct ENGINE_API sSkybox final
        {
            sGeometry Geometry;
            sGeometryInfo GeometryInfo;
            sTexture* Texture;
        };
        JSON(sSkybox, Geometry)

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
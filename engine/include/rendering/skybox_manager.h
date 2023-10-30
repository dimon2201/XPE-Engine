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

        class ENGINE_API SkyboxManager : public res::JsonObject
        {

        public:
            render::Skybox Skybox;

            JsonClass(
                SkyboxManager,
                SkyboxManager::Skybox
            )

            static void Init();
            static void Free();
            static SkyboxManager& Get();

        private:
            static SkyboxManager* s_Instance;
        };

    }

}
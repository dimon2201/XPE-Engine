#pragma once

#include <ecs/components.hpp>

namespace xpe
{
    namespace render
    {
        class ENGINE_API MMaterial final
        {

        public:
            static void Init();
            static void Free();

            static void Update(cScene* scene);

            static CMaterial2 AddMaterial(const sAtlas2DTexture& diffuse);

        };
    }

}
#pragma once

#include <rendering/draw/drawer.h>
#include <geometry/geometries.h>

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        class GeometryStorage;

        class ENGINE_API PostProcessDrawer : public Drawer
        {

        public:
            PostProcessDrawer(
                CameraBuffer* cameraBuffer,
                Shader* shader,
                GeometryStorage* geometryStorage,
                RenderTarget* renderTarget
            );

            ~PostProcessDrawer() override;

            void Draw(Scene* scene);

        private:
            Ref<GeometryIndexed<Vertex2D>> m_Quad;

        };

    }

}
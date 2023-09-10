#pragma once

#include <rendering/draw/drawer.h>
#include <geometry/geometries.h>

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        using namespace ecs;

        class GeometryStorage;

        class ENGINE_API CanvasOutputDrawer : public Drawer
        {

        public:
            CanvasOutputDrawer(CameraBuffer* cameraBuffer, Shader* shader, GeometryStorage* geometryStorage, RenderTarget* renderTarget);
            virtual ~CanvasOutputDrawer();

            void Draw(Scene* scene) override;

        private:
            RenderTarget m_Output;
            Ref<GeometryIndexed<Vertex2D>> m_Quad;

        };

    }

}
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

        class ENGINE_API SSAODrawer : public Drawer
        {

        public:
            SSAODrawer(
                CameraBuffer* cameraBuffer,
                Shader* shader,
                GeometryStorage* geometryStorage,
                Texture* positionTexture,
                Texture* depthTexture,
                RenderTarget* renderTarget
            );

            ~SSAODrawer() override;

            void Draw(Scene* scene);

        private:
            TextureSampler m_Sampler;
            Ref<GeometryIndexed<Vertex2D>> m_Quad;

        };

    }

}
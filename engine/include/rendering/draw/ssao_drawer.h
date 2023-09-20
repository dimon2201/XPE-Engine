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

        struct SSAOBufferLayout
        {
            float SSAODirectionCount;
            float SSAOSampleCount;
            float _pad[2];
        };

        class ENGINE_API SSAODrawer : public Drawer
        {

        public:
            SSAODrawer(
                CameraBuffer* cameraBuffer,
                Shader* shader,
                GeometryStorage* geometryStorage,
                Texture* positionTexture,
                Texture* normalTexture,
                Texture* depthTexture,
                RenderTarget* renderTarget
            );

            ~SSAODrawer() override;

            void Draw(Scene* scene);

        private:
            Buffer m_Buffer;
            SSAOBufferLayout m_BufferData;
            Ref<GeometryIndexed<Vertex2D>> m_Quad;

        };

    }

}
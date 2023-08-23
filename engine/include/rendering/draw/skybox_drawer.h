#pragma once

#include <rendering/draw/drawer.h>

#include <rendering/buffers/vertex_buffer.h>
#include <rendering/buffers/index_buffer.h>

#include <geometry/vertices.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace ecs;
        using namespace math;

        class GeometryStorage;

        class ENGINE_API SkyboxDrawer : public Drawer
        {

        public:
            SkyboxDrawer(CameraBuffer* cameraBuffer, Shader* shader, GeometryStorage* geometryStorage);
            ~SkyboxDrawer() override;

            void Draw(Scene* scene) override;

        protected:
            TextureSampler m_Sampler;
        };

    }

}
#include <rendering/draw/text2d_drawer.h>
#include <rendering/buffers/viewport_buffer.h>
#include <rendering/storages/geometry_storage.h>
#include <rendering/font/font.hpp>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        Text2DDrawer::Text2DDrawer(CameraBuffer *cameraBuffer, Shader *shader, RenderTarget* renderTarget, GeometryStorage *geometryStorage, ViewportBuffer* viewportBuffer)
        : TextDrawer(cameraBuffer, shader, renderTarget, geometryStorage)
        {
            m_Pipeline->VSBuffers.emplace_back(viewportBuffer);
        }

        Text2DDrawer::~Text2DDrawer() {}

        void Text2DDrawer::Draw(Scene *scene)
        {
            scene->EachComponent<Text2DComponent>([this](Text2DComponent* component)
            {
                DrawText(component->Transform, component->Text, component->Font);
            });
        }

    }

}
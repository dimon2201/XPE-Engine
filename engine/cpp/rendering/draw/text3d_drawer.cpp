#include <rendering/draw/text3d_drawer.h>
#include <rendering/storages/geometry_storage.h>
#include <rendering/font/font.hpp>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        Text3DDrawer::Text3DDrawer(
            Shader* shader, RenderTarget* renderTarget,
            GeometryStorage* geometryStorage,
            const vector<Buffer*>& VSBuffers,
            const vector<Buffer*>& PSBuffers
        ) : TextDrawer(shader, renderTarget, geometryStorage, VSBuffers, PSBuffers) {}

        Text3DDrawer::~Text3DDrawer() {}

        void Text3DDrawer::Draw(Scene *scene)
        {
            scene->EachComponent<Text3DComponent>([this](Text3DComponent* component)
            {
                DrawText(component->Transform, component->Text, component->Font);
            });
        }

    }

}
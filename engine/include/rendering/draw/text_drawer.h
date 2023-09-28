#pragma once

#include <rendering/draw/drawer.h>
#include <rendering/buffers/transform_buffer.h>
#include <rendering/buffers/text_buffer.h>

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        using namespace core;
        using namespace math;
        using namespace ecs;

        class GeometryStorage;
        class Font;
        class TextBuffer;

        class ENGINE_API TextDrawer : public Drawer
        {

        public:

            TextDrawer(
                Shader* shader,
                RenderTarget* renderTarget,
                GeometryStorage* geometryStorage,
                const vector<Buffer*>& VSBuffers = {},
                const vector<Buffer*>& PSBuffers = {}
            );

            ~TextDrawer() override;

        protected:
            void DrawText(const Transform& transform, const string& text, const Ref<Font>& font);

            TextBuffer m_TextBuffer;
            TransformBuffer m_TransformBuffer;
        };

    }

}
#pragma once

#include <rendering/buffers/widget_buffer.h>

namespace xpe {

    namespace render {

        using namespace math;
        using namespace res;

        struct ENGINE_API sWidget : public cObject, public cXml
        {
            bool Visible = true;
            sTransform Transform;
            glm::vec4 Color = { 1, 1, 1, 0.5 };
            glm::vec4 ColorHover = { 0.5, 0.5, 0.5, 1.0 };
            glm::vec4 ColorPressed = { 0.25, 0.25, 0.25, 1.0 };
            sTextureLayer Texture;
            bool EnableTexture = false;
            std::function<void()> CallbackHover = [](){};
            std::function<void()> CallbackPressed = [](){};
            std::function<void()> CallbackReleased = [](){};
            std::function<void(sWidgetData&)> CallbackUpdate = [](sWidgetData& widgetData){};
            vector<sWidget*> Children;

            XmlClass(
                "Widget",
                Visible,
                Transform,
                Color,
                ColorHover,
                ColorPressed,
                EnableTexture
            )

            sWidget() {
                Transform.Position = { 0.5, 0.5, 0 };
                Transform.Scale = { 0.5, 0.5, 0 };
            }

            void UpdateXmlChildren();
        };

        class ENGINE_API cWidgetManager final
        {

        public:
            static void Init();
            static void Free();
            static void Clear();
            static void Bind(sPipeline& pipeline);

            template<typename... Args>
            static sWidget AddWidget(Args&&... args);

        private:
            static sSampler s_Sampler;
            static sTexture* s_TextureAtlas;
        };

        template<typename... Args>
        sWidget cWidgetManager::AddWidget(Args &&... args)
        {
            sWidget widget(std::forward<Args>(args)...);
            widget.TextureIndex = s_TextureAtlas->Layers.size();
            if (layer.Pixels == nullptr) {
                layer = texture.CreateLayer();
            } else {
//                layer.ResizePixels(texture.Format, texture.Width, texture.Height);
            }
            texture.Layers.emplace_back(layer);

//            if (layer.Mips.empty()) {
//                layer.GenerateMips(texture.Format, texture.Width, texture.Height);
//            }

            texture.Flush();
            return widget;
        }

    }

}
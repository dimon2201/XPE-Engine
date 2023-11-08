#pragma once

#include <rendering/widget_manager.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sTextLabel : sWidget
        {
            sTextLabel(sFont* font, const string& text = "", const glm::vec4& color = { 1, 1, 1, 1 });

            XmlClass(
                "TextLabel",
                Visible,
                Transform,
                Color,
                ColorHover,
                ColorPressed,
                EnableTexture,
                Text
            )
        };

        struct ENGINE_API sButton : sWidget
        {
            float Corners;
            float Padding;

            sButton();

            XmlClass(
                "Button",
                Visible,
                Transform,
                Color,
                ColorHover,
                ColorPressed,
                EnableTexture,
                Corners,
                Padding,
                Text
            )
        };

        struct ENGINE_API sTextInput : sWidget
        {
            sTextInput(sFont* font, const string& text = "", const glm::vec4& color = { 1, 1, 1, 1 });

            XmlClass(
                "TextInput",
                Visible,
                Transform,
                Color,
                ColorHover,
                ColorPressed,
                EnableTexture,
                Text
            )
        };

    }

}
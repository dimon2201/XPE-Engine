#pragma once

#include <rendering/widget_manager.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sButton : sWidget
        {
            float Corners;
            float Padding;
            string Text;
            glm::vec4 TextColor = { 1, 1, 1, 1 };

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
                Text,
                TextColor
            )
        };

        struct ENGINE_API sEditText : sWidget
        {
            string Text;
            glm::vec4 TextColor = { 1, 1, 1, 1 };
            glm::vec4 HintColor = { 1, 1, 1, 1 };

            sEditText();

            XmlClass(
                "EditText",
                Visible,
                Transform,
                Color,
                ColorHover,
                ColorPressed,
                EnableTexture,
                Text,
                TextColor,
                HintColor
            )
        };

        struct ENGINE_API sLabel : sWidget
        {
            string Text;
            glm::vec4 TextColor = { 1, 1, 1, 1 };

            sLabel();

            XmlClass(
                "Label",
                Visible,
                Transform,
                Color,
                ColorHover,
                ColorPressed,
                EnableTexture,
                Text,
                TextColor
            )
        };

    }

}
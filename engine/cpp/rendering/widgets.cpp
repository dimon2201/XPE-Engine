#include <rendering/widgets.h>

namespace xpe {

    namespace render {

        sTextLabel::sTextLabel(sFont* font, const string &text, const glm::vec4 &color) : sWidget()
        {
            Font = font;
            Text = text;
            Color = color;
            FillFrame = false;
        }

        sButton::sButton() : sWidget() {}

        sTextInput::sTextInput(sFont* font, const string &text, const glm::vec4 &color) : sWidget()
        {
            Font = font;
            Text = text;
            Color = color;
        }

    }

}
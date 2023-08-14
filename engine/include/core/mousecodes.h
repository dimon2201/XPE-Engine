#pragma once

namespace xpe {

    namespace core {

        struct ENGINE_API MouseState final
        {
            bool Pressed = false;
            bool Released = false;
            bool Hold = false;
        };

        enum eMouse : u8
        {
            NoneButton             = 16,

            Button0                = 0,
            Button1                = 1,
            Button2                = 2,
            Button3                = 3,
            Button4                = 4,
            Button5                = 5,
            Button6                = 6,
            Button7                = 7,

            ButtonLast             = Button7,
            ButtonLeft             = Button0,
            ButtonRight            = Button1,
            ButtonMiddle           = Button2
        };

    }

}
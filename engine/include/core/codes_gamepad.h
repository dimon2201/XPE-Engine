#pragma once

namespace xpe {

    namespace core {

        struct ENGINE_API sGamepadState final
        {
            bool Pressed = false;
            bool Released = false;
            bool Hold = false;
        };

        enum eGamepadButton : u8
        {
            Pad_A = 0,
            Pad_B = 1,
            Pad_X = 2,
            Pad_Y = 3,

            Pad_LeftBumper = 4,
            Pad_RightBumper = 5,

            Pad_Back = 6,
            Pad_Start = 7,
            Pad_Guide = 8,

            Pad_LeftThumb = 9,
            Pad_RightThumb = 10,

            Pad_Up = 11,
            Pad_Right = 12,
            Pad_Down = 13,
            Pad_Left = 14,

            Pad_Last = Pad_Left,

            Pad_Cross = A,
            Pad_Circle = B,
            Pad_Square = X,
            Pad_Triangle = Y
        };

        enum eGamepadAxis : u8
        {
            Axis_LeftX = 0,
            Axis_LeftY = 1,
            Axis_RightX = 2,
            Axis_RightY = 3,

            Axis_LeftTrigger = 4,
            Axis_RightTrigger = 5,

            Axis_Last = Axis_RightTrigger
        };

    }

}
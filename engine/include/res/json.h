#pragma once

#include <stl/string.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define Json(clazz, ...) NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(clazz, __VA_ARGS__)

#define JsonClass(clazz, ...)                                   \
                                                                \
NLOHMANN_DEFINE_TYPE_INTRUSIVE(clazz, __VA_ARGS__)              \
                                                                \
clazz() { m_Tag = #clazz; }                                     \
                                                                \
void ToJson(json &root) override                                \
{                                                               \
    root[m_Tag.c_str()] = *this;                                \
}                                                               \
                                                                \
void FromJson(json &root) override                              \
{                                                               \
    root.at(m_Tag.c_str()).get_to(*this);                       \
}                                                               \

#define JsonEnum(clazz, ...) \
NLOHMANN_JSON_SERIALIZE_ENUM(clazz, __VA_ARGS__)

namespace xpe {

    namespace res {

        using namespace core;

        class ENGINE_API JsonObject
        {

        public:

            virtual void ToJson(json& root) = 0;

            virtual void FromJson(json& root) = 0;

            inline void SetTag(const string& tag)
            {
                m_Tag = tag;
            }

            inline const string& GetTag() const
            {
                return m_Tag;
            }

        protected:
            string m_Tag;
        };

        ENGINE_API bool ReadJsonFile(const char* filepath, JsonObject& object);
        ENGINE_API bool WriteJsonFile(const char* filepath, JsonObject& object);

    }

}

// JSON for GLM library
namespace glm {

    Json(vec2, x, y)
    Json(vec3, x, y, z)
    Json(vec4, x, y, z, w)
    Json(ivec2, x, y)
    Json(ivec3, x, y, z)
    Json(ivec4, x, y, z, w)
    Json(quat, x, y, z, w)

    void to_json(json& j, const glm::mat2& m);

    void from_json(const json& j, glm::mat2& m);

    void to_json(json& j, const glm::mat3& m);

    void from_json(const json& j, glm::mat3& m);

    void to_json(json& j, const glm::mat4& m);

    void from_json(const json& j, glm::mat4& m);

} // namespace glm

// JSON for KeyCodes
namespace xpe::core
{

    JsonEnum(eKey, {

        { eKey::None, "None" },
        { eKey::Space, "Space" },
        { eKey::Apostrophe, "Apostrophe" },
        { eKey::Comma, "Comma" },
        { eKey::Minus, "Minus" },
        { eKey::Period, "Period" },
        { eKey::Slash, "Slash" },

        { eKey::D0, "D0" },
        { eKey::D1, "D1" },
        { eKey::D2, "D2" },
        { eKey::D3, "D3" },
        { eKey::D4, "D4" },
        { eKey::D5, "D5" },
        { eKey::D6, "D6" },
        { eKey::D7, "D7" },
        { eKey::D8, "D8" },
        { eKey::D9, "D9" },

        { eKey::Semicolon, "Semicolon" },
        { eKey::Equal, "Equal" },

        { eKey::A, "A" },
        { eKey::B, "B" },
        { eKey::C, "C" },
        { eKey::D, "D" },
        { eKey::E, "E" },
        { eKey::F, "F" },
        { eKey::G, "G" },
        { eKey::H, "H" },
        { eKey::I, "I" },
        { eKey::J, "J" },
        { eKey::K, "K" },
        { eKey::L, "L" },
        { eKey::M, "M" },
        { eKey::N, "N" },
        { eKey::O, "O" },
        { eKey::P, "P" },
        { eKey::Q, "Q" },
        { eKey::R, "R" },
        { eKey::S, "S" },
        { eKey::T, "T" },
        { eKey::U, "U" },
        { eKey::V, "V" },
        { eKey::W, "W" },
        { eKey::X, "X" },
        { eKey::Y, "Y" },
        { eKey::Z, "Z" },

        { eKey::LeftBracket, "LeftBracket" },
        { eKey::Backslash, "Backslash" },
        { eKey::RightBracket, "RightBracket" },
        { eKey::GraveAccent, "GraveAccent" },

        { eKey::World1, "World1" },
        { eKey::World2, "World2" },

        { eKey::Esc, "Esc" },
        { eKey::Enter, "Enter" },
        { eKey::Tab, "Tab" },
        { eKey::Backspace, "Backspace" },

        { eKey::Insert, "Insert" },
        { eKey::Delete, "Delete" },

        { eKey::Right, "Right" },
        { eKey::Left, "Left" },
        { eKey::Down, "Down" },
        { eKey::Up, "Up" },

        { eKey::PageUp, "PageUp" },
        { eKey::PageDown, "PageDown" },
        { eKey::Home, "Home" },
        { eKey::End, "End" },

        { eKey::CapsLock, "CapsLock" },
        { eKey::ScrollLock, "ScrollLock" },
        { eKey::NumLock, "NumLock" },

        { eKey::PrintScreen, "PrintScreen" },
        { eKey::Pause, "Pause" },

        { eKey::F1, "F1" },
        { eKey::F2, "F2" },
        { eKey::F3, "F3" },
        { eKey::F4, "F4" },
        { eKey::F5, "F5" },
        { eKey::F6, "F6" },
        { eKey::F7, "F7" },
        { eKey::F8, "F8" },
        { eKey::F9, "F9" },
        { eKey::F10, "F10" },
        { eKey::F11, "F11" },
        { eKey::F12, "F12" },
        { eKey::F13, "F13" },
        { eKey::F14, "F14" },
        { eKey::F15, "F15" },
        { eKey::F16, "F16" },
        { eKey::F17, "F17" },
        { eKey::F18, "F18" },
        { eKey::F19, "F19" },
        { eKey::F20, "F20" },
        { eKey::F21, "F21" },
        { eKey::F22, "F22" },
        { eKey::F23, "F23" },
        { eKey::F24, "F24" },
        { eKey::F25, "F25" },

        { eKey::KP0, "KP0" },
        { eKey::KP1, "KP1" },
        { eKey::KP2, "KP2" },
        { eKey::KP3, "KP3" },
        { eKey::KP4, "KP4" },
        { eKey::KP5, "KP5" },
        { eKey::KP6, "KP6" },
        { eKey::KP7, "KP7" },
        { eKey::KP8, "KP8" },
        { eKey::KP9, "KP9" },
        { eKey::KPDecimal, "KPDecimal" },
        { eKey::KPDivide, "KPDivide" },
        { eKey::KPMultiply, "KPMultiply" },
        { eKey::KPSubtract, "KPSubtract" },
        { eKey::KPAdd, "KPAdd" },
        { eKey::KPEnter, "KPEnter" },
        { eKey::KPEqual, "KPEqual" },

        { eKey::LeftShift, "LeftShift" },
        { eKey::LeftControl, "LeftControl" },
        { eKey::LeftAlt, "LeftAlt" },
        { eKey::LeftSuper, "LeftSuper" },
        { eKey::RightShift, "RightShift" },
        { eKey::RightControl, "RightControl" },
        { eKey::RightAlt, "RightAlt" },
        { eKey::RightSuper, "RightSuper" },

        { eKey::Menu, "Menu" },
    })

} // namespace xpe::core

// JSON for MouseCodes
namespace xpe::core
{

    JsonEnum(eMouse, {

        { eMouse::NoneButton, "NoneButton" },

        { eMouse::Button0, "Button0" },
        { eMouse::Button1, "Button1" },
        { eMouse::Button2, "Button2" },
        { eMouse::Button3, "Button3" },
        { eMouse::Button4, "Button4" },
        { eMouse::Button5, "Button5" },
        { eMouse::Button6, "Button6" },
        { eMouse::Button7, "Button7" },

        { eMouse::ButtonLast, "ButtonLast" },
        { eMouse::ButtonLeft, "ButtonLeft" },
        { eMouse::ButtonRight, "ButtonRight" },
        { eMouse::ButtonMiddle, "ButtonMiddle" },

    })

} // namespace xpe::core

// JSON for GamepadCodes
namespace xpe::core
{

    JsonEnum(eGamepadButton, {

        { eGamepadButton::Pad_A, "Pad_A" },
        { eGamepadButton::Pad_B, "Pad_B" },
        { eGamepadButton::Pad_X, "Pad_X" },
        { eGamepadButton::Pad_Y, "Pad_Y" },

        { eGamepadButton::Pad_LeftBumper, "Pad_LeftBumper" },
        { eGamepadButton::Pad_RightBumper, "Pad_RightBumper" },

        { eGamepadButton::Pad_Back, "Pad_Back" },
        { eGamepadButton::Pad_Start, "Pad_Start" },
        { eGamepadButton::Pad_Guide, "Pad_Guide" },

        { eGamepadButton::Pad_LeftThumb, "Pad_LeftThumb" },
        { eGamepadButton::Pad_RightThumb, "Pad_RightThumb" },

        { eGamepadButton::Pad_Up, "Pad_Up" },
        { eGamepadButton::Pad_Right, "Pad_Right" },
        { eGamepadButton::Pad_Down, "Pad_Down" },
        { eGamepadButton::Pad_Left, "Pad_Left" },

        { eGamepadButton::Pad_Last, "Pad_Last" },

        { eGamepadButton::Pad_Cross, "Pad_Cross" },
        { eGamepadButton::Pad_Circle, "Pad_Circle" },
        { eGamepadButton::Pad_Square, "Pad_Square" },
        { eGamepadButton::Pad_Triangle, "Pad_Triangle" },
    })

    JsonEnum(eGamepadAxis, {

        { eGamepadAxis::Axis_LeftX, "Axis_LeftX" },
        { eGamepadAxis::Axis_LeftY, "Axis_LeftY" },
        { eGamepadAxis::Axis_RightX, "Axis_RightX" },
        { eGamepadAxis::Axis_RightY, "Axis_RightY" },

        { eGamepadAxis::Axis_LeftTrigger, "Axis_LeftTrigger" },
        { eGamepadAxis::Axis_RightTrigger, "Axis_RightTrigger" },

        { eGamepadAxis::Axis_Last, "Axis_Last" }

    })

} // namespace xpe::core
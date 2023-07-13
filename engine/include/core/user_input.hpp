#pragma once

#include <core/events.h>

namespace xpe {

    namespace core {

        struct ENGINE_API MouseCursor final {
            glm::vec2 Position = { 0, 0 };
            glm::vec2 Delta = { 0, 0 };
        };

        class ENGINE_API Input final {

        public:
            static EventBuffer<WindowClosedFn> WindowClosedEvents;
            static EventBuffer<WindowResizedFn> WindowResizedEvents;
            static EventBuffer<WindowFrameResizedFn> WindowFrameResizedEvents;
            static EventBuffer<WindowMovedFn> WindowMovedEvents;
            static EventBuffer<WindowFocusedFn> WindowFocusedEvents;
            static EventBuffer<WindowFocusLostFn> WindowFocusLostEvents;

            static EventBuffer<KeyPressedFn> KeyPressedEvents;
            static EventBuffer<KeyReleasedFn> KeyReleasedEvents;
            static EventBuffer<KeyHoldFn> KeyHoldEvents;

            static EventBuffer<MousePressedFn> MousePressedEvents;
            static EventBuffer<MouseReleasedFn> MouseReleasedEvents;
            static EventBuffer<MouseHoldFn> MouseHoldEvents;

            static EventBuffer<CursorMovedFn> CursorMovedEvents;
            static EventBuffer<CursorEnteredFn> CursorEnteredEvents;
            static EventBuffer<CursorLeftFn> CursorLeftEvents;

            static EventBuffer<ScrollChangedFn> ScrollChangedEvents;

            static EventBuffer<CharTypedFn> CharTypedEvents;
            static EventBuffer<CharModsTypedFn> CharModsTypedEvents;

        public:
            static void Init();
            static void Free();

            static void CaptureCursor();
            static void CaptureCursor(const double x, const double y);

            static bool KeyPressed(const eKey key);
            static bool KeyReleased(const eKey key);
            static bool KeyHold(const eKey key);
            static KeyState GetKeyState(const eKey key);

            static bool MousePressed(const eMouse mouse);
            static bool MouseReleased(const eMouse mouse);
            static bool MouseHold(const eMouse mouse);
            static MouseState GetMouseState(const eMouse mouse);

            static inline const MouseCursor& GetMouseCursor() {
                return s_Cursor;
            }

        private:
            static void InitWindowCallbacks();
            static void InitKeyCallbacks();
            static void InitMouseCallbacks();
            static void InitCursorCallbacks();
            static void InitScrollCallbacks();
            static void InitCharCallbacks();

        private:
            static MouseCursor s_Cursor;

        };

    }

}
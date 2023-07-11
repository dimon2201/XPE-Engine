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

            static void AddWindowEventListener(WindowEventListener* listener, int priority);
            static void AddKeyEventListener(KeyEventListener* listener, int priority);
            static void AddMouseEventListener(MouseEventListener* listener, int priority);
            static void AddCursorEventListener(CursorEventListener* listener, int priority);
            static void AddScrollEventListener(ScrollEventListener* listener, int priority);
            static void AddCharEventListener(CharEventListener* listener, int priority);

            static void RemoveWindowEventListener(WindowEventListener* listener);
            static void RemoveKeyEventListener(KeyEventListener* listener);
            static void RemoveMouseEventListener(MouseEventListener* listener);
            static void RemoveCursorEventListener(CursorEventListener* listener);
            static void RemoveScrollEventListener(ScrollEventListener* listener);
            static void RemoveCharEventListener(CharEventListener* listener);

            static inline const EventBuffer<WindowEventListener>& GetWindowEvents() {
                return s_WindowEvents;
            }

            static inline const EventBuffer<KeyEventListener>& GetKeyEvents() {
                return s_KeyEvents;
            }

            static inline const EventBuffer<MouseEventListener>& GetMouseEvents() {
                return s_MouseEvents;
            }

            static inline const EventBuffer<CursorEventListener>& GetCursorEvents() {
                return s_CursorEvents;
            }

            static inline const EventBuffer<ScrollEventListener>& GetScrollEvents() {
                return s_ScrollEvents;
            }

            static inline const EventBuffer<CharEventListener>& GetCharEvents() {
                return s_CharEvents;
            }

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
            static EventBuffer<WindowEventListener> s_WindowEvents;
            static EventBuffer<KeyEventListener> s_KeyEvents;
            static EventBuffer<MouseEventListener> s_MouseEvents;
            static EventBuffer<CursorEventListener> s_CursorEvents;
            static EventBuffer<ScrollEventListener> s_ScrollEvents;
            static EventBuffer<CharEventListener> s_CharEvents;
        };

    }

}
#pragma once

#include <core/events.h>

namespace xpe {

    namespace core {

        struct ENGINE_API MouseCursor final {
            glm::vec2 Position;
            glm::vec2 Delta;
        };

        class ENGINE_API cUserInputManager : public Object {

        public:
            cUserInputManager() = default;
            ~cUserInputManager() = default;

            void Init(void* window);
            void Free();

            void CaptureCursor();

            bool isKeyPressed(const eKey key);
            bool isKeyReleased(const eKey key);
            bool isKeyHold(const eKey key);
            KeyState GetKeyState(const eKey key);

            bool isMousePressed(const eMouse mouse);
            bool isMouseReleased(const eMouse mouse);
            bool isMouseHold(const eMouse mouse);
            MouseState GetMouseState(const eMouse mouse);

            void AddWindowEventListener(WindowEventListener* listener, int priority);
            void AddKeyEventListener(KeyEventListener* listener, int priority);
            void AddMouseEventListener(MouseEventListener* listener, int priority);
            void AddCursorEventListener(CursorEventListener* listener, int priority);

            void RemoveWindowEventListener(WindowEventListener* listener);
            void RemoveKeyEventListener(KeyEventListener* listener);
            void RemoveMouseEventListener(MouseEventListener* listener);
            void RemoveCursorEventListener(CursorEventListener* listener);

            inline const EventBuffer<WindowEventListener>& GetWindowEvents() const {
                return m_WindowEvents;
            }

            inline const EventBuffer<KeyEventListener>& GetKeyEvents() const {
                return m_KeyEvents;
            }

            inline const EventBuffer<MouseEventListener>& GetMouseEvents() const {
                return m_MouseEvents;
            }

            inline const EventBuffer<CursorEventListener>& GetCursorEvents() const {
                return m_CursorEvents;
            }

            inline const MouseCursor& GetMouseCursor() {
                return m_Cursor;
            }

        private:
            void InitWindowCallbacks();
            void InitKeyCallbacks();
            void InitMouseCallbacks();
            void InitCursorCallbacks();

        private:
            MouseCursor m_Cursor;
            EventBuffer<WindowEventListener> m_WindowEvents;
            EventBuffer<KeyEventListener> m_KeyEvents;
            EventBuffer<MouseEventListener> m_MouseEvents;
            EventBuffer<CursorEventListener> m_CursorEvents;
        };

    }

}
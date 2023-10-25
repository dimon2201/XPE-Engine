#pragma once

#include <core/events.h>

#define AddWindowClosed(clazz, priority) xpe::core::InputManager::WindowClosedEvents->AddEvent(this, xpe::core::OnWindowClosed<clazz>, priority)
#define AddWindowFrameResized(clazz, priority) xpe::core::InputManager::WindowFrameResizedEvents->AddEvent(this, xpe::core::OnWindowFrameResized<clazz>, priority)

#define AddKeyPressed(clazz, priority) xpe::core::InputManager::KeyPressedEvents->AddEvent(this, xpe::core::OnKeyPressed<clazz>, priority)
#define AddKeyRelease(clazz, priority) xpe::core::InputManager::KeyReleasedEvents->AddEvent(this, xpe::core::OnKeyReleased<clazz>, priority)
#define AddKeyHold(clazz, priority) xpe::core::InputManager::KeyHoldEvents->AddEvent(this, xpe::core::OnKeyHold<clazz>, priority)

#define AddCursorMove(clazz, priority) xpe::core::InputManager::CursorMovedEvents->AddEvent(this, xpe::core::OnCursorMoved<clazz>, priority)

#define AddScrollChanged(clazz, priority) xpe::core::InputManager::ScrollChangedEvents->AddEvent(this, xpe::core::OnScrollChanged<clazz>, priority)

#define RemoveWindowClose() xpe::core::InputManager::WindowClosedEvents->RemoveEvent(this)
#define RemoveWindowFrameResized() xpe::core::InputManager::WindowFrameResizedEvents->RemoveEvent(this)

#define RemoveKeyPressed() xpe::core::InputManager::KeyPressedEvents->RemoveEvent(this)
#define RemoveKeyRelease() xpe::core::InputManager::KeyReleasedEvents->RemoveEvent(this)
#define RemoveKeyHold() xpe::core::InputManager::KeyHoldEvents->RemoveEvent(this)

#define RemoveCursorMove() xpe::core::InputManager::CursorMovedEvents->RemoveEvent(this)

#define RemoveScrollChanged() xpe::core::InputManager::ScrollChangedEvents->RemoveEvent(this)

namespace xpe {

    namespace core {

        struct ENGINE_API MouseCursor final {
            glm::vec2 Position = { 0, 0 };
            glm::vec2 Delta = { 0, 0 };
        };

        class ENGINE_API InputManager final {

        public:
            static EventBuffer<WindowClosedFn>* WindowClosedEvents;
            static EventBuffer<WindowResizedFn>* WindowResizedEvents;
            static EventBuffer<WindowFrameResizedFn>* WindowFrameResizedEvents;
            static EventBuffer<WindowMovedFn>* WindowMovedEvents;
            static EventBuffer<WindowFocusedFn>* WindowFocusedEvents;
            static EventBuffer<WindowFocusLostFn>* WindowFocusLostEvents;

            static EventBuffer<KeyPressedFn>* KeyPressedEvents;
            static EventBuffer<KeyReleasedFn>* KeyReleasedEvents;
            static EventBuffer<KeyHoldFn>* KeyHoldEvents;

            static EventBuffer<MousePressedFn>* MousePressedEvents;
            static EventBuffer<MouseReleasedFn>* MouseReleasedEvents;
            static EventBuffer<MouseHoldFn>* MouseHoldEvents;

            static EventBuffer<CursorMovedFn>* CursorMovedEvents;
            static EventBuffer<CursorEnteredFn>* CursorEnteredEvents;
            static EventBuffer<CursorLeftFn>* CursorLeftEvents;

            static EventBuffer<ScrollChangedFn>* ScrollChangedEvents;

            static EventBuffer<CharTypedFn>* CharTypedEvents;
            static EventBuffer<CharModsTypedFn>* CharModsTypedEvents;

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
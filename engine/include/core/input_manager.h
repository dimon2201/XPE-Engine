#pragma once

#include <core/events.h>

#define AddWindowClosed(clazz, priority) xpe::core::EventManager::Get().WindowClosedEvents.AddEvent(this, xpe::core::OnWindowClosed<clazz>, priority)
#define AddWindowFrameResized(clazz, priority) xpe::core::EventManager::Get().WindowFrameResizedEvents.AddEvent(this, xpe::core::OnWindowFrameResized<clazz>, priority)

#define AddKeyPressed(clazz, priority) xpe::core::EventManager::Get().KeyPressedEvents.AddEvent(this, xpe::core::OnKeyPressed<clazz>, priority)
#define AddKeyRelease(clazz, priority) xpe::core::EventManager::Get().KeyReleasedEvents.AddEvent(this, xpe::core::OnKeyReleased<clazz>, priority)
#define AddKeyHold(clazz, priority) xpe::core::EventManager::Get().KeyHoldEvents.AddEvent(this, xpe::core::OnKeyHold<clazz>, priority)

#define AddCursorMove(clazz, priority) xpe::core::EventManager::Get().CursorMovedEvents.AddEvent(this, xpe::core::OnCursorMoved<clazz>, priority)

#define AddScrollChanged(clazz, priority) xpe::core::EventManager::Get().ScrollChangedEvents.AddEvent(this, xpe::core::OnScrollChanged<clazz>, priority)

#define RemoveWindowClose() xpe::core::EventManager::Get().WindowClosedEvents.RemoveEvent(this)
#define RemoveWindowFrameResized() xpe::core::EventManager::Get().WindowFrameResizedEvents.RemoveEvent(this)

#define RemoveKeyPressed() xpe::core::EventManager::Get().KeyPressedEvents.RemoveEvent(this)
#define RemoveKeyRelease() xpe::core::EventManager::Get().KeyReleasedEvents.RemoveEvent(this)
#define RemoveKeyHold() xpe::core::EventManager::Get().KeyHoldEvents.RemoveEvent(this)

#define RemoveCursorMove() xpe::core::EventManager::Get().CursorMovedEvents.RemoveEvent(this)

#define RemoveScrollChanged() xpe::core::EventManager::Get().ScrollChangedEvents.RemoveEvent(this)

namespace xpe {

    namespace core {

        struct ENGINE_API MouseCursor final {
            glm::vec2 Position = { 0, 0 };
            glm::vec2 Delta = { 0, 0 };
        };

        class ENGINE_API InputManager final {

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
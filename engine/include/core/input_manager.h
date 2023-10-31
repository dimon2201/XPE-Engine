#pragma once

#include <core/events.h>

#define AddWindowClosed(clazz, priority) xpe::core::cEventManager::Get().WindowClosedEvents.AddEvent(this, xpe::core::OnWindowClosed<clazz>, priority)
#define AddWindowFrameResized(clazz, priority) xpe::core::cEventManager::Get().WindowFrameResizedEvents.AddEvent(this, xpe::core::OnWindowFrameResized<clazz>, priority)

#define AddKeyPressed(clazz, priority) xpe::core::cEventManager::Get().KeyPressedEvents.AddEvent(this, xpe::core::OnKeyPressed<clazz>, priority)
#define AddKeyRelease(clazz, priority) xpe::core::cEventManager::Get().KeyReleasedEvents.AddEvent(this, xpe::core::OnKeyReleased<clazz>, priority)
#define AddKeyHold(clazz, priority) xpe::core::cEventManager::Get().KeyHoldEvents.AddEvent(this, xpe::core::OnKeyHold<clazz>, priority)

#define AddCursorMove(clazz, priority) xpe::core::cEventManager::Get().CursorMovedEvents.AddEvent(this, xpe::core::OnCursorMoved<clazz>, priority)

#define AddScrollChanged(clazz, priority) xpe::core::cEventManager::Get().ScrollChangedEvents.AddEvent(this, xpe::core::OnScrollChanged<clazz>, priority)

#define RemoveWindowClose() xpe::core::cEventManager::Get().WindowClosedEvents.RemoveEvent(this)
#define RemoveWindowFrameResized() xpe::core::cEventManager::Get().WindowFrameResizedEvents.RemoveEvent(this)

#define RemoveKeyPressed() xpe::core::cEventManager::Get().KeyPressedEvents.RemoveEvent(this)
#define RemoveKeyRelease() xpe::core::cEventManager::Get().KeyReleasedEvents.RemoveEvent(this)
#define RemoveKeyHold() xpe::core::cEventManager::Get().KeyHoldEvents.RemoveEvent(this)

#define RemoveCursorMove() xpe::core::cEventManager::Get().CursorMovedEvents.RemoveEvent(this)

#define RemoveScrollChanged() xpe::core::cEventManager::Get().ScrollChangedEvents.RemoveEvent(this)

namespace xpe {

    namespace core {

        struct ENGINE_API sMouseCursor final {
            glm::vec2 Position = { 0, 0 };
            glm::vec2 Delta = { 0, 0 };
        };

        class ENGINE_API cInputManager final {

        public:
            static void Init();
            static void Free();

            static void CaptureCursor();
            static void CaptureCursor(const double x, const double y);

            static bool KeyPressed(const eKey key);
            static bool KeyReleased(const eKey key);
            static bool KeyHold(const eKey key);
            static sKeyState GetKeyState(const eKey key);

            static bool MousePressed(const eMouse mouse);
            static bool MouseReleased(const eMouse mouse);
            static bool MouseHold(const eMouse mouse);
            static sMouseState GetMouseState(const eMouse mouse);

            static inline const sMouseCursor& GetMouseCursor() {
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
            static sMouseCursor s_Cursor;

        };

    }

}
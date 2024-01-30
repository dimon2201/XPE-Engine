#pragma once

namespace xpe
{
    namespace core
    {
        struct ENGINE_API sMouseCursor final
        {
            glm::vec2 Position = { 0, 0 };
            glm::vec2 Delta = { 0, 0 };
        };

        class ENGINE_API MInput final
        {

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

            static inline const sMouseCursor& GetMouseCursor()
            {
                return s_Cursor;
            }

        private:
            static void InitWindowCallbacks();
            static void InitKeyCallbacks();
            static void InitMouseCallbacks();
            static void InitCursorCallbacks();
            static void InitScrollCallbacks();
            static void InitCharCallbacks();

            static sMouseCursor s_Cursor;
        };
    }
}
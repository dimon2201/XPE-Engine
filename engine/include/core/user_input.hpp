#pragma once

struct GLFWwindow;

namespace xpe
{
    namespace core
    {
        class Window;

        void ENGINE_API _KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void ENGINE_API _MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

        enum class eKeyState
        {
            NONE = 0,
            PRESSED = 1,
            RELEASED = 2
        };

        enum class eMouseButton
        {
            LEFT = 0,
            MIDDLE = 1,
            RIGHT = 2
        };

        class ENGINE_API cUserInputManager
        {
            // This callbacks are used to capture user input
            friend void _KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
            friend void _MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

            public:
                cUserInputManager();
                ~cUserInputManager();

                void Init(Window* window);
                void CaptureCursor(GLFWwindow* window);

                inline eKeyState GetKey(int key) { return _keys[key]; }
                inline boolean GetKeyPressed(int key) { return _keys[key] == eKeyState::PRESSED ? K_TRUE : K_FALSE; }
                inline boolean GetKeyReleased(int key) { return _keys[key] == eKeyState::RELEASED ? K_TRUE : K_FALSE; }
                inline glm::vec2 GetMouseCursorPosition() { return _cursorPosition; }
                inline glm::vec2 GetMouseCursorPositionDelta() { return _cursorPositionDelta; }
                inline eKeyState GetMouseButton(const eMouseButton& mb) { return _mb[(usize)mb]; }
                inline boolean GetMouseButtonPressed(const eMouseButton& mb) { return _mb[(usize)mb] == eKeyState::PRESSED ? K_TRUE : K_FALSE; }
                inline boolean GetMouseButtonReleased(const eMouseButton& mb) { return _mb[(usize)mb] == eKeyState::RELEASED ? K_TRUE : K_FALSE; } 

            private:
                eKeyState _keys[256];
                glm::vec2 _cursorPosition;
                glm::vec2 _cursorPositionDelta;
                eKeyState _mb[3];
        };
    }
}
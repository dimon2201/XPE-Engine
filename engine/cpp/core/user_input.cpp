#include <glfw3.h>

#include <core/user_input.hpp>
#include <core/windowing.hpp>

namespace xpe {

    namespace core {

        static GLFWwindow* s_Window = nullptr;

        MouseCursor Input::s_Cursor;

        EventBuffer<WindowClosedFn> Input::WindowClosedEvents;
        EventBuffer<WindowResizedFn> Input::WindowResizedEvents;
        EventBuffer<WindowFrameResizedFn> Input::WindowFrameResizedEvents;
        EventBuffer<WindowMovedFn> Input::WindowMovedEvents;
        EventBuffer<WindowFocusedFn> Input::WindowFocusedEvents;
        EventBuffer<WindowFocusLostFn> Input::WindowFocusLostEvents;

        EventBuffer<KeyPressedFn> Input::KeyPressedEvents;
        EventBuffer<KeyReleasedFn> Input::KeyReleasedEvents;
        EventBuffer<KeyHoldFn> Input::KeyHoldEvents;

        EventBuffer<MousePressedFn> Input::MousePressedEvents;
        EventBuffer<MouseReleasedFn> Input::MouseReleasedEvents;
        EventBuffer<MouseHoldFn> Input::MouseHoldEvents;

        EventBuffer<CursorMovedFn> Input::CursorMovedEvents;
        EventBuffer<CursorEnteredFn> Input::CursorEnteredEvents;
        EventBuffer<CursorLeftFn> Input::CursorLeftEvents;

        EventBuffer<ScrollChangedFn> Input::ScrollChangedEvents;

        EventBuffer<CharTypedFn> Input::CharTypedEvents;
        EventBuffer<CharModsTypedFn> Input::CharModsTypedEvents;

        void Input::Init() {
            s_Window = static_cast<GLFWwindow*>(WindowManager::GetInstance());

            InitWindowCallbacks();
            InitKeyCallbacks();
            InitMouseCallbacks();
            InitCursorCallbacks();
            InitScrollCallbacks();
            InitCharCallbacks();
        }

        void Input::InitWindowCallbacks() {
            glfwSetWindowCloseCallback(s_Window, [](GLFWwindow* window) {
                for (const auto& event : Input::WindowClosedEvents.GetEvents()) {
                    event.Function(event.Thiz);
                }
            });

            glfwSetWindowPosCallback(s_Window, [](GLFWwindow* window, int x, int y) {
                for (const auto& event : Input::WindowMovedEvents.GetEvents()) {
                    event.Function(event.Thiz, x, y);
                }
            });

            glfwSetWindowSizeCallback(s_Window, [](GLFWwindow* window, int w, int h) {
                for (const auto& event : Input::WindowResizedEvents.GetEvents()) {
                    event.Function(event.Thiz, w, h);
                }
            });

            glfwSetFramebufferSizeCallback(s_Window, [](GLFWwindow* window, int w, int h) {
                for (const auto& event : Input::WindowFrameResizedEvents.GetEvents()) {
                    event.Function(event.Thiz, w, h);
                }
            });

            glfwSetWindowFocusCallback(s_Window, [](GLFWwindow* window, int focused) {
                if (focused == GLFW_TRUE) {
                    for (const auto& event : Input::WindowFocusedEvents.GetEvents()) {
                        event.Function(event.Thiz);
                    }
                } else {
                    for (const auto& event : Input::WindowFocusLostEvents.GetEvents()) {
                        event.Function(event.Thiz);
                    }
                }
            });
        }

        void Input::InitKeyCallbacks() {
            glfwSetKeyCallback(s_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                switch (action) {

                    case GLFW_PRESS:
                        for (const auto& event : Input::KeyPressedEvents.GetEvents()) {
                            event.Function(event.Thiz, static_cast<eKey>(key));
                        }
                        break;

                    case GLFW_RELEASE:
                        for (const auto& event : Input::KeyReleasedEvents.GetEvents()) {
                            event.Function(event.Thiz, static_cast<eKey>(key));
                        }
                        break;

                    case GLFW_REPEAT:
                        for (const auto& event : Input::KeyHoldEvents.GetEvents()) {
                            event.Function(event.Thiz, static_cast<eKey>(key));
                        }
                        break;

                }
            });
        }

        void Input::InitMouseCallbacks() {
            glfwSetMouseButtonCallback(s_Window, [](GLFWwindow* window, int button, int action, int mods) {
                switch (action) {

                    case GLFW_PRESS:
                        for (const auto& event : Input::MousePressedEvents.GetEvents()) {
                            event.Function(event.Thiz, static_cast<eMouse>(button));
                        }
                        break;

                    case GLFW_RELEASE:
                        for (const auto& event : Input::MouseReleasedEvents.GetEvents()) {
                            event.Function(event.Thiz, static_cast<eMouse>(button));
                        }
                        break;

                    case GLFW_REPEAT:
                        for (const auto& event : Input::MouseHoldEvents.GetEvents()) {
                            event.Function(event.Thiz, static_cast<eMouse>(button));
                        }
                        break;

                }
            });
        }

        void Input::InitCursorCallbacks() {
            glfwSetCursorPosCallback(s_Window, [](GLFWwindow* window, double x, double y) {
                for (const auto& event : Input::CursorMovedEvents.GetEvents()) {
                    event.Function(event.Thiz, x, y);
                }
            });

            glfwSetCursorEnterCallback(s_Window, [](GLFWwindow* window, int entered) {
                if (entered == GLFW_TRUE) {
                    for (const auto& event : Input::CursorEnteredEvents.GetEvents()) {
                        event.Function(event.Thiz);
                    }
                } else {
                    for (const auto& event : Input::CursorLeftEvents.GetEvents()) {
                        event.Function(event.Thiz);
                    }
                }
            });
        }

        void Input::InitScrollCallbacks() {
            glfwSetScrollCallback(s_Window, [](GLFWwindow* window, double x, double y) {
                for (const auto& event : Input::ScrollChangedEvents.GetEvents()) {
                    event.Function(event.Thiz, x, y);
                }
            });
        }

        void Input::InitCharCallbacks() {
            glfwSetCharCallback(s_Window, [](GLFWwindow* window, u32 charUnicode) {
                for (const auto& event : Input::CharTypedEvents.GetEvents()) {
                    event.Function(event.Thiz, charUnicode);
                }
            });

            glfwSetCharModsCallback(s_Window, [](GLFWwindow* window, u32 charUnicode, int mods) {
                for (const auto& event : Input::CharModsTypedEvents.GetEvents()) {
                    event.Function(event.Thiz, charUnicode, mods);
                }
            });
        }

        void Input::Free() {
            WindowClosedEvents.Clear();
            WindowResizedEvents.Clear();
            WindowFrameResizedEvents.Clear();
            WindowMovedEvents.Clear();
            WindowFocusedEvents.Clear();
            WindowFocusLostEvents.Clear();

            KeyPressedEvents.Clear();
            KeyReleasedEvents.Clear();
            KeyHoldEvents.Clear();

            MousePressedEvents.Clear();
            MouseReleasedEvents.Clear();
            MouseHoldEvents.Clear();
        }

        void Input::CaptureCursor() {
            double xPos = 0.0;
            double yPos = 0.0;
            glfwGetCursorPos(s_Window, &xPos, &yPos);
            CaptureCursor(xPos, yPos);
        }

        void Input::CaptureCursor(const double x, const double y) {
//            LogInfo("{} {}", x, y);

            glm::vec2 cursorPosition = { x, y };

//            if (x >= WindowManager::GetWidth()) {
//                s_Cursor.Delta.x += 1;
//                s_Cursor.Position = cursorPosition;
//                return;
//            }
//
//            if (y >= WindowManager::GetHeight()) {
//                s_Cursor.Delta.y += 1;
//                s_Cursor.Position = cursorPosition;
//                return;
//            }

            s_Cursor.Delta = cursorPosition - s_Cursor.Position;
            s_Cursor.Position = cursorPosition;
        }

        bool Input::KeyPressed(const eKey key) {
            return glfwGetKey(s_Window, key) == GLFW_PRESS;
        }

        bool Input::KeyReleased(const eKey key) {
            return glfwGetKey(s_Window, key) == GLFW_RELEASE;
        }

        bool Input::KeyHold(const eKey key) {
            return glfwGetKey(s_Window, key) == GLFW_REPEAT;
        }

        KeyState Input::GetKeyState(const eKey key) {
            int state = glfwGetKey(s_Window, key);
            KeyState keyState;
            keyState.Pressed = state == GLFW_PRESS;
            keyState.Released = state == GLFW_RELEASE;
            keyState.Hold = state == GLFW_REPEAT;
            return keyState;
        }

        bool Input::MousePressed(const eMouse mouse) {
            return glfwGetMouseButton(s_Window, mouse) == GLFW_PRESS;
        }

        bool Input::MouseReleased(const eMouse mouse) {
            return glfwGetMouseButton(s_Window, mouse) == GLFW_RELEASE;
        }

        bool Input::MouseHold(const eMouse mouse) {
            return glfwGetMouseButton(s_Window, mouse) == GLFW_REPEAT;
        }

        MouseState Input::GetMouseState(const eMouse mouse) {
            int state = glfwGetMouseButton(s_Window, mouse);
            MouseState mouseState;
            mouseState.Pressed = state == GLFW_PRESS;
            mouseState.Released = state == GLFW_RELEASE;
            mouseState.Hold = state == GLFW_REPEAT;
            return mouseState;
        }

    }

}
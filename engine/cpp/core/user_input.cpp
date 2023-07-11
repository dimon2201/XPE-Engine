#include <glfw3.h>

#include <core/user_input.hpp>
#include <core/windowing.hpp>

namespace xpe {

    namespace core {

        static GLFWwindow* s_Window = nullptr;

        MouseCursor Input::s_Cursor;
        EventBuffer<WindowEventListener> Input::s_WindowEvents;
        EventBuffer<KeyEventListener> Input::s_KeyEvents;
        EventBuffer<MouseEventListener> Input::s_MouseEvents;
        EventBuffer<CursorEventListener> Input::s_CursorEvents;
        EventBuffer<ScrollEventListener> Input::s_ScrollEvents;
        EventBuffer<CharEventListener> Input::s_CharEvents;

        void Input::Init() {
            s_Window = static_cast<GLFWwindow*>(WindowManager::GetInstance());

            s_WindowEvents.Reserve(5);
            s_KeyEvents.Reserve(5);
            s_MouseEvents.Reserve(5);

            InitWindowCallbacks();
            InitKeyCallbacks();
            InitMouseCallbacks();
            InitCursorCallbacks();
            InitScrollCallbacks();
            InitCharCallbacks();
        }

        void Input::InitWindowCallbacks() {
            glfwSetWindowCloseCallback(s_Window, [](GLFWwindow* window) {
                for (const auto& event : Input::GetWindowEvents().GetEvents()) {
                    event.Listener->WindowClosed();
                }
            });

            glfwSetWindowPosCallback(s_Window, [](GLFWwindow* window, int x, int y) {
                for (const auto& event : Input::GetWindowEvents().GetEvents()) {
                    event.Listener->WindowMoved(x, y);
                }
            });

            glfwSetWindowSizeCallback(s_Window, [](GLFWwindow* window, int w, int h) {
                for (const auto& event : Input::GetWindowEvents().GetEvents()) {
                    event.Listener->WindowResized(w, h);
                }
            });

            glfwSetFramebufferSizeCallback(s_Window, [](GLFWwindow* window, int w, int h) {
                for (const auto& event : Input::GetWindowEvents().GetEvents()) {
                    event.Listener->WindowFrameResized(w, h);
                }
            });

            glfwSetWindowFocusCallback(s_Window, [](GLFWwindow* window, int focused) {
                if (focused == GLFW_TRUE) {
                    for (const auto& event : Input::GetWindowEvents().GetEvents()) {
                        event.Listener->WindowFocused();
                    }
                } else {
                    for (const auto& event : Input::GetWindowEvents().GetEvents()) {
                        event.Listener->WindowFocusLost();
                    }
                }
            });
        }

        void Input::InitKeyCallbacks() {
            glfwSetKeyCallback(s_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                switch (action) {

                    case GLFW_PRESS:
                        for (const auto& event : Input::GetKeyEvents().GetEvents()) {
                            event.Listener->KeyPressed(static_cast<eKey>(key));
                        }
                        break;

                    case GLFW_RELEASE:
                        for (const auto& event : Input::GetKeyEvents().GetEvents()) {
                            event.Listener->KeyReleased(static_cast<eKey>(key));
                        }
                        break;

                    case GLFW_REPEAT:
                        for (const auto& event : Input::GetKeyEvents().GetEvents()) {
                            event.Listener->KeyHold(static_cast<eKey>(key));
                        }
                        break;

                }
            });
        }

        void Input::InitMouseCallbacks() {
            glfwSetMouseButtonCallback(s_Window, [](GLFWwindow* window, int button, int action, int mods) {
                switch (action) {

                    case GLFW_PRESS:
                        for (const auto& event : Input::GetMouseEvents().GetEvents()) {
                            event.Listener->MousePressed(static_cast<eMouse>(button));
                        }
                        break;

                    case GLFW_RELEASE:
                        for (const auto& event : Input::GetMouseEvents().GetEvents()) {
                            event.Listener->MouseReleased(static_cast<eMouse>(button));
                        }
                        break;

                    case GLFW_REPEAT:
                        for (const auto& event : Input::GetMouseEvents().GetEvents()) {
                            event.Listener->MouseHold(static_cast<eMouse>(button));
                        }
                        break;

                }
            });
        }

        void Input::InitCursorCallbacks() {
            glfwSetCursorPosCallback(s_Window, [](GLFWwindow* window, double x, double y) {
                for (const auto& event : Input::GetCursorEvents().GetEvents()) {
                    event.Listener->CursorMoved(x, y);
                }
            });

            glfwSetCursorEnterCallback(s_Window, [](GLFWwindow* window, int entered) {
                if (entered == GLFW_TRUE) {
                    for (const auto& event : Input::GetCursorEvents().GetEvents()) {
                        event.Listener->CursorEntered();
                    }
                } else {
                    for (const auto& event : Input::GetCursorEvents().GetEvents()) {
                        event.Listener->CursorLeft();
                    }
                }
            });
        }

        void Input::InitScrollCallbacks() {
            glfwSetScrollCallback(s_Window, [](GLFWwindow* window, double x, double y) {
                for (const auto& event : Input::GetScrollEvents().GetEvents()) {
                    event.Listener->ScrollChanged(x, y);
                }
            });
        }

        void Input::InitCharCallbacks() {
            glfwSetCharCallback(s_Window, [](GLFWwindow* window, u32 charUnicode) {
                for (const auto& event : Input::GetCharEvents().GetEvents()) {
                    event.Listener->CharTyped(charUnicode);
                }
            });

            glfwSetCharModsCallback(s_Window, [](GLFWwindow* window, u32 charUnicode, int mods) {
                for (const auto& event : Input::GetCharEvents().GetEvents()) {
                    event.Listener->CharModsTyped(charUnicode, mods);
                }
            });
        }

        void Input::Free() {
            s_WindowEvents.Clear();
            s_KeyEvents.Clear();
            s_MouseEvents.Clear();
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

        void Input::AddWindowEventListener(WindowEventListener* listener, int priority) {
            s_WindowEvents.AddEvent(listener, priority);
        }

        void Input::AddKeyEventListener(KeyEventListener* listener, int priority) {
            s_KeyEvents.AddEvent(listener, priority);
        }

        void Input::AddMouseEventListener(MouseEventListener* listener, int priority) {
            s_MouseEvents.AddEvent(listener, priority);
        }

        void Input::AddCursorEventListener(CursorEventListener* listener, int priority) {
            s_CursorEvents.AddEvent(listener, priority);
        }

        void Input::AddScrollEventListener(ScrollEventListener *listener, int priority) {
            s_ScrollEvents.AddEvent(listener, priority);
        }

        void Input::AddCharEventListener(CharEventListener *listener, int priority) {
            s_CharEvents.AddEvent(listener, priority);
        }

        void Input::RemoveWindowEventListener(WindowEventListener* listener) {
            s_WindowEvents.RemoveEvent(listener);
        }

        void Input::RemoveKeyEventListener(KeyEventListener* listener) {
            s_KeyEvents.RemoveEvent(listener);
        }

        void Input::RemoveMouseEventListener(MouseEventListener* listener) {
            s_MouseEvents.RemoveEvent(listener);
        }

        void Input::RemoveCursorEventListener(CursorEventListener* listener) {
            s_CursorEvents.RemoveEvent(listener);
        }

        void Input::RemoveScrollEventListener(ScrollEventListener *listener) {
            s_ScrollEvents.RemoveEvent(listener);
        }

        void Input::RemoveCharEventListener(CharEventListener *listener) {
            s_CharEvents.RemoveEvent(listener);
        }

    }

}
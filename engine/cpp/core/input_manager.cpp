#include <glfw3.h>

namespace xpe {

    namespace core {

        static GLFWwindow* s_Window = nullptr;

        MouseCursor InputManager::s_Cursor;

        void InputManager::Init() {
            LogInfo("Input::Init()");

            s_Window = static_cast<GLFWwindow*>(WindowManager::GetInstance());

            InitWindowCallbacks();
            InitKeyCallbacks();
            InitMouseCallbacks();
            InitCursorCallbacks();
            InitScrollCallbacks();
            InitCharCallbacks();

            LogInfo("Input initialized");
        }

        void InputManager::InitWindowCallbacks() {
            glfwSetWindowCloseCallback(s_Window, [](GLFWwindow* window) {
                EventManager::Get().WindowClosedEvents.NotifyAll();
            });

            glfwSetWindowPosCallback(s_Window, [](GLFWwindow* window, int x, int y) {
                EventManager::Get().WindowMovedEvents.NotifyAll(x, y);
            });

            glfwSetWindowSizeCallback(s_Window, [](GLFWwindow* window, int w, int h) {
                if (w <= 0 || h <= 0) {
                    LogWarning("Window size has invalid width={} height={}. WindowResized event will not be handled!", w, h);
                    return;
                }
                EventManager::Get().WindowFrameResizedEvents.NotifyAll(w, h);
            });

            glfwSetFramebufferSizeCallback(s_Window, [](GLFWwindow* window, int w, int h) {
                if (w <= 0 || h <= 0) {
                    LogWarning("Window framebuffer size has invalid width={} height={}. FramebufferResized event will not be handled!", w, h);
                    return;
                }
                EventManager::Get().WindowFrameResizedEvents.NotifyAll(w, h);
            });

            glfwSetWindowFocusCallback(s_Window, [](GLFWwindow* window, int focused) {
                if (focused == GLFW_TRUE) {
                    EventManager::Get().WindowFocusedEvents.NotifyAll();
                } else {
                    EventManager::Get().WindowFocusLostEvents.NotifyAll();
                }
            });
        }

        void InputManager::InitKeyCallbacks() {
            glfwSetKeyCallback(s_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                switch (action) {

                    case GLFW_PRESS:
                        EventManager::Get().KeyPressedEvents.NotifyAll(static_cast<eKey>(key));
                        break;

                    case GLFW_RELEASE:
                        EventManager::Get().KeyReleasedEvents.NotifyAll(static_cast<eKey>(key));
                        break;

                    case GLFW_REPEAT:
                        EventManager::Get().KeyHoldEvents.NotifyAll(static_cast<eKey>(key));
                        break;

                }
            });
        }

        void InputManager::InitMouseCallbacks() {
            glfwSetMouseButtonCallback(s_Window, [](GLFWwindow* window, int button, int action, int mods) {
                switch (action) {

                    case GLFW_PRESS:
                        EventManager::Get().MousePressedEvents.NotifyAll(static_cast<eMouse>(button));
                        break;

                    case GLFW_RELEASE:
                        EventManager::Get().MouseReleasedEvents.NotifyAll(static_cast<eMouse>(button));
                        break;

                    case GLFW_REPEAT:
                        EventManager::Get().MouseHoldEvents.NotifyAll(static_cast<eMouse>(button));
                        break;

                }
            });
        }

        void InputManager::InitCursorCallbacks() {
            glfwSetCursorPosCallback(s_Window, [](GLFWwindow* window, double x, double y) {
                EventManager::Get().CursorMovedEvents.NotifyAll(x, y);
            });

            glfwSetCursorEnterCallback(s_Window, [](GLFWwindow* window, int entered) {
                if (entered == GLFW_TRUE) {
                    EventManager::Get().CursorEnteredEvents.NotifyAll();
                } else {
                    EventManager::Get().CursorLeftEvents.NotifyAll();
                }
            });
        }

        void InputManager::InitScrollCallbacks() {
            glfwSetScrollCallback(s_Window, [](GLFWwindow* window, double x, double y) {
                EventManager::Get().ScrollChangedEvents.NotifyAll(x, y);
            });
        }

        void InputManager::InitCharCallbacks() {
            glfwSetCharCallback(s_Window, [](GLFWwindow* window, u32 charUnicode) {
                EventManager::Get().CharTypedEvents.NotifyAll(charUnicode);
            });

            glfwSetCharModsCallback(s_Window, [](GLFWwindow* window, u32 charUnicode, int mods) {
                EventManager::Get().CharModsTypedEvents.NotifyAll(charUnicode, mods);
            });
        }

        void InputManager::Free() {}

        void InputManager::CaptureCursor() {
            double xPos = 0.0;
            double yPos = 0.0;
            glfwGetCursorPos(s_Window, &xPos, &yPos);
            CaptureCursor(xPos, yPos);
        }

        void InputManager::CaptureCursor(const double x, const double y) {
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

        bool InputManager::KeyPressed(const eKey key) {
            return glfwGetKey(s_Window, key) == GLFW_PRESS;
        }

        bool InputManager::KeyReleased(const eKey key) {
            return glfwGetKey(s_Window, key) == GLFW_RELEASE;
        }

        bool InputManager::KeyHold(const eKey key) {
            return glfwGetKey(s_Window, key) == GLFW_REPEAT;
        }

        KeyState InputManager::GetKeyState(const eKey key) {
            int state = glfwGetKey(s_Window, key);
            KeyState keyState;
            keyState.Pressed = state == GLFW_PRESS;
            keyState.Released = state == GLFW_RELEASE;
            keyState.Hold = state == GLFW_REPEAT;
            return keyState;
        }

        bool InputManager::MousePressed(const eMouse mouse) {
            return glfwGetMouseButton(s_Window, mouse) == GLFW_PRESS;
        }

        bool InputManager::MouseReleased(const eMouse mouse) {
            return glfwGetMouseButton(s_Window, mouse) == GLFW_RELEASE;
        }

        bool InputManager::MouseHold(const eMouse mouse) {
            return glfwGetMouseButton(s_Window, mouse) == GLFW_REPEAT;
        }

        MouseState InputManager::GetMouseState(const eMouse mouse) {
            int state = glfwGetMouseButton(s_Window, mouse);
            MouseState mouseState;
            mouseState.Pressed = state == GLFW_PRESS;
            mouseState.Released = state == GLFW_RELEASE;
            mouseState.Hold = state == GLFW_REPEAT;
            return mouseState;
        }

    }

}
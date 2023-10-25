#include <glfw3.h>

namespace xpe {

    namespace core {

        static GLFWwindow* s_Window = nullptr;

        MouseCursor InputManager::s_Cursor;

        EventBuffer<WindowClosedFn>* InputManager::WindowClosedEvents = nullptr;
        EventBuffer<WindowResizedFn>* InputManager::WindowResizedEvents = nullptr;
        EventBuffer<WindowFrameResizedFn>* InputManager::WindowFrameResizedEvents = nullptr;
        EventBuffer<WindowMovedFn>* InputManager::WindowMovedEvents = nullptr;
        EventBuffer<WindowFocusedFn>* InputManager::WindowFocusedEvents = nullptr;
        EventBuffer<WindowFocusLostFn>* InputManager::WindowFocusLostEvents = nullptr;

        EventBuffer<KeyPressedFn>* InputManager::KeyPressedEvents = nullptr;
        EventBuffer<KeyReleasedFn>* InputManager::KeyReleasedEvents = nullptr;
        EventBuffer<KeyHoldFn>* InputManager::KeyHoldEvents = nullptr;

        EventBuffer<MousePressedFn>* InputManager::MousePressedEvents = nullptr;
        EventBuffer<MouseReleasedFn>* InputManager::MouseReleasedEvents = nullptr;
        EventBuffer<MouseHoldFn>* InputManager::MouseHoldEvents = nullptr;

        EventBuffer<CursorMovedFn>* InputManager::CursorMovedEvents = nullptr;
        EventBuffer<CursorEnteredFn>* InputManager::CursorEnteredEvents = nullptr;
        EventBuffer<CursorLeftFn>* InputManager::CursorLeftEvents = nullptr;

        EventBuffer<ScrollChangedFn>* InputManager::ScrollChangedEvents = nullptr;

        EventBuffer<CharTypedFn>* InputManager::CharTypedEvents = nullptr;
        EventBuffer<CharModsTypedFn>* InputManager::CharModsTypedEvents = nullptr;

        void InputManager::Init() {
            LogInfo("Input::Init()");

            s_Window = static_cast<GLFWwindow*>(WindowManager::GetInstance());

            WindowClosedEvents = new EventBuffer<WindowClosedFn>;
            WindowResizedEvents = new EventBuffer<WindowResizedFn>;
            WindowFrameResizedEvents = new EventBuffer<WindowFrameResizedFn>;
            WindowMovedEvents = new EventBuffer<WindowMovedFn>;
            WindowFocusedEvents = new EventBuffer<WindowFocusedFn>;
            WindowFocusLostEvents = new EventBuffer<WindowFocusLostFn>;

            KeyPressedEvents = new EventBuffer<KeyPressedFn>;
            KeyReleasedEvents = new EventBuffer<KeyReleasedFn>;
            KeyHoldEvents = new EventBuffer<KeyHoldFn>;

            MousePressedEvents = new EventBuffer<MousePressedFn>;
            MouseReleasedEvents = new EventBuffer<MouseReleasedFn>;
            MouseHoldEvents = new EventBuffer<MouseHoldFn>;

            CursorMovedEvents = new EventBuffer<CursorMovedFn>;
            CursorEnteredEvents = new EventBuffer<CursorEnteredFn>;
            CursorLeftEvents = new EventBuffer<CursorLeftFn>;

            ScrollChangedEvents = new EventBuffer<ScrollChangedFn>;

            CharTypedEvents = new EventBuffer<CharTypedFn>;
            CharModsTypedEvents = new EventBuffer<CharModsTypedFn>;

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
                InputManager::WindowClosedEvents->NotifyAll();
            });

            glfwSetWindowPosCallback(s_Window, [](GLFWwindow* window, int x, int y) {
                InputManager::WindowMovedEvents->NotifyAll(x, y);
            });

            glfwSetWindowSizeCallback(s_Window, [](GLFWwindow* window, int w, int h) {
                if (w <= 0 || h <= 0) {
                    LogWarning("Window size has invalid width={} height={}. WindowResized event will not be handled!", w, h);
                    return;
                }
                InputManager::WindowFrameResizedEvents->NotifyAll(w, h);
            });

            glfwSetFramebufferSizeCallback(s_Window, [](GLFWwindow* window, int w, int h) {
                if (w <= 0 || h <= 0) {
                    LogWarning("Window framebuffer size has invalid width={} height={}. FramebufferResized event will not be handled!", w, h);
                    return;
                }
                InputManager::WindowFrameResizedEvents->NotifyAll(w, h);
            });

            glfwSetWindowFocusCallback(s_Window, [](GLFWwindow* window, int focused) {
                if (focused == GLFW_TRUE) {
                    InputManager::WindowFocusedEvents->NotifyAll();
                } else {
                    InputManager::WindowFocusLostEvents->NotifyAll();
                }
            });
        }

        void InputManager::InitKeyCallbacks() {
            glfwSetKeyCallback(s_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                switch (action) {

                    case GLFW_PRESS:
                        InputManager::KeyPressedEvents->NotifyAll(static_cast<eKey>(key));
                        break;

                    case GLFW_RELEASE:
                        InputManager::KeyReleasedEvents->NotifyAll(static_cast<eKey>(key));
                        break;

                    case GLFW_REPEAT:
                        InputManager::KeyHoldEvents->NotifyAll(static_cast<eKey>(key));
                        break;

                }
            });
        }

        void InputManager::InitMouseCallbacks() {
            glfwSetMouseButtonCallback(s_Window, [](GLFWwindow* window, int button, int action, int mods) {
                switch (action) {

                    case GLFW_PRESS:
                        InputManager::MousePressedEvents->NotifyAll(static_cast<eMouse>(button));
                        break;

                    case GLFW_RELEASE:
                        InputManager::MouseReleasedEvents->NotifyAll(static_cast<eMouse>(button));
                        break;

                    case GLFW_REPEAT:
                        InputManager::MouseHoldEvents->NotifyAll(static_cast<eMouse>(button));
                        break;

                }
            });
        }

        void InputManager::InitCursorCallbacks() {
            glfwSetCursorPosCallback(s_Window, [](GLFWwindow* window, double x, double y) {
                InputManager::CursorMovedEvents->NotifyAll(x, y);
            });

            glfwSetCursorEnterCallback(s_Window, [](GLFWwindow* window, int entered) {
                if (entered == GLFW_TRUE) {
                    InputManager::CursorEnteredEvents->NotifyAll();
                } else {
                    InputManager::CursorLeftEvents->NotifyAll();
                }
            });
        }

        void InputManager::InitScrollCallbacks() {
            glfwSetScrollCallback(s_Window, [](GLFWwindow* window, double x, double y) {
                InputManager::ScrollChangedEvents->NotifyAll(x, y);
            });
        }

        void InputManager::InitCharCallbacks() {
            glfwSetCharCallback(s_Window, [](GLFWwindow* window, u32 charUnicode) {
                InputManager::CharTypedEvents->NotifyAll(charUnicode);
            });

            glfwSetCharModsCallback(s_Window, [](GLFWwindow* window, u32 charUnicode, int mods) {
                InputManager::CharModsTypedEvents->NotifyAll(charUnicode, mods);
            });
        }

        void InputManager::Free() {
            delete WindowClosedEvents;
            delete WindowResizedEvents;
            delete WindowFrameResizedEvents;
            delete WindowMovedEvents;
            delete WindowFocusedEvents;
            delete WindowFocusLostEvents;

            delete KeyPressedEvents;
            delete KeyReleasedEvents;
            delete KeyHoldEvents;

            delete MousePressedEvents;
            delete MouseReleasedEvents;
            delete MouseHoldEvents;

            delete CursorMovedEvents;
            delete CursorEnteredEvents;
            delete CursorLeftEvents;

            delete ScrollChangedEvents;

            delete CharTypedEvents;
            delete CharModsTypedEvents;
        }

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
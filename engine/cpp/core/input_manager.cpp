#include <glfw3.h>

namespace xpe {

    namespace core {

        static GLFWwindow* s_Window = nullptr;

        sMouseCursor cInputManager::s_Cursor;

        void cInputManager::Init() {
            LogInfo("Input::Init()");

            s_Window = static_cast<GLFWwindow*>(cWindowManager::GetInstance());

            InitWindowCallbacks();
            InitKeyCallbacks();
            InitMouseCallbacks();
            InitCursorCallbacks();
            InitScrollCallbacks();
            InitCharCallbacks();

            LogInfo("Input initialized");
        }

        void cInputManager::InitWindowCallbacks() {
            glfwSetWindowCloseCallback(s_Window, [](GLFWwindow* window) {
                cEventManager::Get().WindowClosedEvents.NotifyAll();
            });

            glfwSetWindowPosCallback(s_Window, [](GLFWwindow* window, int x, int y) {
                cEventManager::Get().WindowMovedEvents.NotifyAll(x, y);
            });

            glfwSetWindowSizeCallback(s_Window, [](GLFWwindow* window, int w, int h) {
                if (w <= 0 || h <= 0) {
                    LogWarning("Window size has invalid width={} height={}. WindowResized event will not be handled!", w, h);
                    return;
                }
                cEventManager::Get().WindowResizedEvents.NotifyAll(w, h);
            });

            glfwSetFramebufferSizeCallback(s_Window, [](GLFWwindow* window, int w, int h) {
                if (w <= 0 || h <= 0) {
                    LogWarning("Window framebuffer size has invalid width={} height={}. FramebufferResized event will not be handled!", w, h);
                    return;
                }
                cEventManager::Get().WindowFrameResizedEvents.NotifyAll(w, h);
            });

            glfwSetWindowFocusCallback(s_Window, [](GLFWwindow* window, int focused) {
                if (focused == GLFW_TRUE) {
                    cEventManager::Get().WindowFocusedEvents.NotifyAll();
                } else {
                    cEventManager::Get().WindowFocusLostEvents.NotifyAll();
                }
            });
        }

        void cInputManager::InitKeyCallbacks() {
            glfwSetKeyCallback(s_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                switch (action) {

                    case GLFW_PRESS:
                        cEventManager::Get().KeyPressedEvents.NotifyAll(static_cast<eKey>(key));
                        break;

                    case GLFW_RELEASE:
                        cEventManager::Get().KeyReleasedEvents.NotifyAll(static_cast<eKey>(key));
                        break;

                    case GLFW_REPEAT:
                        cEventManager::Get().KeyHoldEvents.NotifyAll(static_cast<eKey>(key));
                        break;

                }
            });
        }

        void cInputManager::InitMouseCallbacks() {
            glfwSetMouseButtonCallback(s_Window, [](GLFWwindow* window, int button, int action, int mods) {
                switch (action) {

                    case GLFW_PRESS:
                        cEventManager::Get().MousePressedEvents.NotifyAll(static_cast<eMouse>(button));
                        break;

                    case GLFW_RELEASE:
                        cEventManager::Get().MouseReleasedEvents.NotifyAll(static_cast<eMouse>(button));
                        break;

                    case GLFW_REPEAT:
                        cEventManager::Get().MouseHoldEvents.NotifyAll(static_cast<eMouse>(button));
                        break;

                }
            });
        }

        void cInputManager::InitCursorCallbacks() {
            glfwSetCursorPosCallback(s_Window, [](GLFWwindow* window, double x, double y) {
                cEventManager::Get().CursorMovedEvents.NotifyAll(x, y);
            });

            glfwSetCursorEnterCallback(s_Window, [](GLFWwindow* window, int entered) {
                if (entered == GLFW_TRUE) {
                    cEventManager::Get().CursorEnteredEvents.NotifyAll();
                } else {
                    cEventManager::Get().CursorLeftEvents.NotifyAll();
                }
            });
        }

        void cInputManager::InitScrollCallbacks() {
            glfwSetScrollCallback(s_Window, [](GLFWwindow* window, double x, double y) {
                cEventManager::Get().ScrollChangedEvents.NotifyAll(x, y);
            });
        }

        void cInputManager::InitCharCallbacks() {
            glfwSetCharCallback(s_Window, [](GLFWwindow* window, u32 charUnicode) {
                cEventManager::Get().CharTypedEvents.NotifyAll(charUnicode);
            });

            glfwSetCharModsCallback(s_Window, [](GLFWwindow* window, u32 charUnicode, int mods) {
                cEventManager::Get().CharModsTypedEvents.NotifyAll(charUnicode, mods);
            });
        }

        void cInputManager::Free() {}

        void cInputManager::CaptureCursor() {
            double xPos = 0.0;
            double yPos = 0.0;
            glfwGetCursorPos(s_Window, &xPos, &yPos);
            CaptureCursor(xPos, yPos);
        }

        void cInputManager::CaptureCursor(const double x, const double y) {
//            LogInfo("{} {}", x, y);

            glm::vec2 cursorPosition = { x, y };

//            if (x >= cWindowManager::GetWidth()) {
//                s_Cursor.Delta.x += 1;
//                s_Cursor.Position = cursorPosition;
//                return;
//            }
//
//            if (y >= cWindowManager::GetHeight()) {
//                s_Cursor.Delta.y += 1;
//                s_Cursor.Position = cursorPosition;
//                return;
//            }

            s_Cursor.Delta = cursorPosition - s_Cursor.Position;
            s_Cursor.Position = cursorPosition;
        }

        bool cInputManager::KeyPressed(const eKey key) {
            return glfwGetKey(s_Window, key) == GLFW_PRESS;
        }

        bool cInputManager::KeyReleased(const eKey key) {
            return glfwGetKey(s_Window, key) == GLFW_RELEASE;
        }

        bool cInputManager::KeyHold(const eKey key) {
            return glfwGetKey(s_Window, key) == GLFW_REPEAT;
        }

        sKeyState cInputManager::GetKeyState(const eKey key) {
            int state = glfwGetKey(s_Window, key);
            sKeyState keyState;
            keyState.Pressed = state == GLFW_PRESS;
            keyState.Released = state == GLFW_RELEASE;
            keyState.Hold = state == GLFW_REPEAT;
            return keyState;
        }

        bool cInputManager::MousePressed(const eMouse mouse) {
            return glfwGetMouseButton(s_Window, mouse) == GLFW_PRESS;
        }

        bool cInputManager::MouseReleased(const eMouse mouse) {
            return glfwGetMouseButton(s_Window, mouse) == GLFW_RELEASE;
        }

        bool cInputManager::MouseHold(const eMouse mouse) {
            return glfwGetMouseButton(s_Window, mouse) == GLFW_REPEAT;
        }

        sMouseState cInputManager::GetMouseState(const eMouse mouse) {
            int state = glfwGetMouseButton(s_Window, mouse);
            sMouseState mouseState;
            mouseState.Pressed = state == GLFW_PRESS;
            mouseState.Released = state == GLFW_RELEASE;
            mouseState.Hold = state == GLFW_REPEAT;
            return mouseState;
        }

    }

}
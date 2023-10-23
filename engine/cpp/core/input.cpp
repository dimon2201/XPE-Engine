#include <glfw3.h>

#include <core/input.h>
#include <core/windowing.hpp>

namespace xpe {

    namespace core {

        static GLFWwindow* s_Window = nullptr;

        MouseCursor Input::s_Cursor;

        EventBuffer<WindowClosedFn>* Input::WindowClosedEvents = nullptr;
        EventBuffer<WindowResizedFn>* Input::WindowResizedEvents = nullptr;
        EventBuffer<WindowFrameResizedFn>* Input::WindowFrameResizedEvents = nullptr;
        EventBuffer<WindowMovedFn>* Input::WindowMovedEvents = nullptr;
        EventBuffer<WindowFocusedFn>* Input::WindowFocusedEvents = nullptr;
        EventBuffer<WindowFocusLostFn>* Input::WindowFocusLostEvents = nullptr;

        EventBuffer<KeyPressedFn>* Input::KeyPressedEvents = nullptr;
        EventBuffer<KeyReleasedFn>* Input::KeyReleasedEvents = nullptr;
        EventBuffer<KeyHoldFn>* Input::KeyHoldEvents = nullptr;

        EventBuffer<MousePressedFn>* Input::MousePressedEvents = nullptr;
        EventBuffer<MouseReleasedFn>* Input::MouseReleasedEvents = nullptr;
        EventBuffer<MouseHoldFn>* Input::MouseHoldEvents = nullptr;

        EventBuffer<CursorMovedFn>* Input::CursorMovedEvents = nullptr;
        EventBuffer<CursorEnteredFn>* Input::CursorEnteredEvents = nullptr;
        EventBuffer<CursorLeftFn>* Input::CursorLeftEvents = nullptr;

        EventBuffer<ScrollChangedFn>* Input::ScrollChangedEvents = nullptr;

        EventBuffer<CharTypedFn>* Input::CharTypedEvents = nullptr;
        EventBuffer<CharModsTypedFn>* Input::CharModsTypedEvents = nullptr;

        void Input::Init() {
            LogInfo("Input::Init()");

            s_Window = static_cast<GLFWwindow*>(Windowing::GetInstance());

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

        void Input::InitWindowCallbacks() {
            glfwSetWindowCloseCallback(s_Window, [](GLFWwindow* window) {
                Input::WindowClosedEvents->NotifyAll();
            });

            glfwSetWindowPosCallback(s_Window, [](GLFWwindow* window, int x, int y) {
                Input::WindowMovedEvents->NotifyAll(x, y);
            });

            glfwSetWindowSizeCallback(s_Window, [](GLFWwindow* window, int w, int h) {
                if (w <= 0 || h <= 0) {
                    LogWarning("Window size has invalid width={} height={}. WindowResized event will not be handled!", w, h);
                    return;
                }
                Input::WindowFrameResizedEvents->NotifyAll(w, h);
            });

            glfwSetFramebufferSizeCallback(s_Window, [](GLFWwindow* window, int w, int h) {
                if (w <= 0 || h <= 0) {
                    LogWarning("Window framebuffer size has invalid width={} height={}. FramebufferResized event will not be handled!", w, h);
                    return;
                }
                Input::WindowFrameResizedEvents->NotifyAll(w, h);
            });

            glfwSetWindowFocusCallback(s_Window, [](GLFWwindow* window, int focused) {
                if (focused == GLFW_TRUE) {
                    Input::WindowFocusedEvents->NotifyAll();
                } else {
                    Input::WindowFocusLostEvents->NotifyAll();
                }
            });
        }

        void Input::InitKeyCallbacks() {
            glfwSetKeyCallback(s_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                switch (action) {

                    case GLFW_PRESS:
                        Input::KeyPressedEvents->NotifyAll(static_cast<eKey>(key));
                        break;

                    case GLFW_RELEASE:
                        Input::KeyReleasedEvents->NotifyAll(static_cast<eKey>(key));
                        break;

                    case GLFW_REPEAT:
                        Input::KeyHoldEvents->NotifyAll(static_cast<eKey>(key));
                        break;

                }
            });
        }

        void Input::InitMouseCallbacks() {
            glfwSetMouseButtonCallback(s_Window, [](GLFWwindow* window, int button, int action, int mods) {
                switch (action) {

                    case GLFW_PRESS:
                        Input::MousePressedEvents->NotifyAll(static_cast<eMouse>(button));
                        break;

                    case GLFW_RELEASE:
                        Input::MouseReleasedEvents->NotifyAll(static_cast<eMouse>(button));
                        break;

                    case GLFW_REPEAT:
                        Input::MouseHoldEvents->NotifyAll(static_cast<eMouse>(button));
                        break;

                }
            });
        }

        void Input::InitCursorCallbacks() {
            glfwSetCursorPosCallback(s_Window, [](GLFWwindow* window, double x, double y) {
                Input::CursorMovedEvents->NotifyAll(x, y);
            });

            glfwSetCursorEnterCallback(s_Window, [](GLFWwindow* window, int entered) {
                if (entered == GLFW_TRUE) {
                    Input::CursorEnteredEvents->NotifyAll();
                } else {
                    Input::CursorLeftEvents->NotifyAll();
                }
            });
        }

        void Input::InitScrollCallbacks() {
            glfwSetScrollCallback(s_Window, [](GLFWwindow* window, double x, double y) {
                Input::ScrollChangedEvents->NotifyAll(x, y);
            });
        }

        void Input::InitCharCallbacks() {
            glfwSetCharCallback(s_Window, [](GLFWwindow* window, u32 charUnicode) {
                Input::CharTypedEvents->NotifyAll(charUnicode);
            });

            glfwSetCharModsCallback(s_Window, [](GLFWwindow* window, u32 charUnicode, int mods) {
                Input::CharModsTypedEvents->NotifyAll(charUnicode, mods);
            });
        }

        void Input::Free() {
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

        void Input::CaptureCursor() {
            double xPos = 0.0;
            double yPos = 0.0;
            glfwGetCursorPos(s_Window, &xPos, &yPos);
            CaptureCursor(xPos, yPos);
        }

        void Input::CaptureCursor(const double x, const double y) {
//            LogInfo("{} {}", x, y);

            glm::vec2 cursorPosition = { x, y };

//            if (x >= Windowing::GetWidth()) {
//                s_Cursor.Delta.x += 1;
//                s_Cursor.Position = cursorPosition;
//                return;
//            }
//
//            if (y >= Windowing::GetHeight()) {
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
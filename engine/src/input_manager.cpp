#include <glfw3.h>

namespace xpe
{
    namespace core
    {
        static GLFWwindow* s_Window = nullptr;

        sMouseCursor MInput::s_Cursor;

        void MInput::Init()
        {
            LogInfo("Input::Init()");

            s_Window = static_cast<GLFWwindow*>(MWindow::GetInstance());

            InitWindowCallbacks();
            InitKeyCallbacks();
            InitMouseCallbacks();
            InitCursorCallbacks();
            InitScrollCallbacks();
            InitCharCallbacks();

            LogInfo("Input initialized");
        }

        void MInput::InitWindowCallbacks()
        {
            glfwSetWindowCloseCallback(s_Window, [](GLFWwindow* window)
            {
                MEvent::WindowClosed.Notify();
            });

            glfwSetWindowPosCallback(s_Window, [](GLFWwindow* window, int x, int y)
            {
                MEvent::WindowMoved.Notify(x, y);
            });

            glfwSetWindowSizeCallback(s_Window, [](GLFWwindow* window, int w, int h)
            {
                if (w <= 0 || h <= 0)
                {
                    LogWarning("Window size has invalid width={} height={}. WindowResized event will not be handled!", w, h);
                    return;
                }
                MEvent::WindowResized.Notify(w, h);
            });

            glfwSetFramebufferSizeCallback(s_Window, [](GLFWwindow* window, int w, int h)
            {
                if (w <= 0 || h <= 0)
                {
                    LogWarning("Window framebuffer size has invalid width={} height={}. FramebufferResized event will not be handled!", w, h);
                    return;
                }
                MEvent::WindowFrameResized.Notify(w, h);
            });

            glfwSetWindowFocusCallback(s_Window, [](GLFWwindow* window, int focused)
            {
                if (focused == GLFW_TRUE)
                {
                    MEvent::WindowFocused.Notify();
                }
                else
                {
                    MEvent::WindowFocusLost.Notify();
                }
            });
        }

        void MInput::InitKeyCallbacks()
        {
            glfwSetKeyCallback(s_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                switch (action)
                {
                    case GLFW_PRESS:
                        MEvent::KeyPressed.Notify(static_cast<eKey>(key));
                        break;

                    case GLFW_RELEASE:
                        MEvent::KeyReleased.Notify(static_cast<eKey>(key));
                        break;

                    case GLFW_REPEAT:
                        MEvent::KeyHold.Notify(static_cast<eKey>(key));
                        break;
                }
            });
        }

        void MInput::InitMouseCallbacks()
        {
            glfwSetMouseButtonCallback(s_Window, [](GLFWwindow* window, int button, int action, int mods)
            {
                switch (action)
                {
                    case GLFW_PRESS:
                        MEvent::MousePressed.Notify(static_cast<eMouse>(button));
                        break;

                    case GLFW_RELEASE:
                        MEvent::MouseReleased.Notify(static_cast<eMouse>(button));
                        break;

                    case GLFW_REPEAT:
                        MEvent::MouseHold.Notify(static_cast<eMouse>(button));
                        break;
                }
            });
        }

        void MInput::InitCursorCallbacks()
        {
            glfwSetCursorPosCallback(s_Window, [](GLFWwindow* window, double x, double y)
            {
                MEvent::CursorMoved.Notify(x, y);
            });

            glfwSetCursorEnterCallback(s_Window, [](GLFWwindow* window, int entered)
            {
                if (entered == GLFW_TRUE)
                {
                    MEvent::CursorEntered.Notify();
                }
                else
                {
                    MEvent::CursorLeft.Notify();
                }
            });
        }

        void MInput::InitScrollCallbacks()
        {
            glfwSetScrollCallback(s_Window, [](GLFWwindow* window, double x, double y)
            {
                MEvent::ScrollChanged.Notify(x, y);
            });
        }

        void MInput::InitCharCallbacks()
        {
            glfwSetCharCallback(s_Window, [](GLFWwindow* window, u32 charUnicode)
            {
                MEvent::CharTyped.Notify(charUnicode);
            });

            glfwSetCharModsCallback(s_Window, [](GLFWwindow* window, u32 charUnicode, int mods)
            {
                MEvent::CharModsTyped.Notify(charUnicode, mods);
            });
        }

        void MInput::Free() {}

        void MInput::CaptureCursor()
        {
            double xPos = 0.0;
            double yPos = 0.0;
            glfwGetCursorPos(s_Window, &xPos, &yPos);
            CaptureCursor(xPos, yPos);
        }

        void MInput::CaptureCursor(const double x, const double y)
        {
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

        bool MInput::KeyPressed(const eKey key)
        {
            return glfwGetKey(s_Window, key) == GLFW_PRESS;
        }

        bool MInput::KeyReleased(const eKey key)
        {
            return glfwGetKey(s_Window, key) == GLFW_RELEASE;
        }

        bool MInput::KeyHold(const eKey key)
        {
            return glfwGetKey(s_Window, key) == GLFW_REPEAT;
        }

        sKeyState MInput::GetKeyState(const eKey key)
        {
            int state = glfwGetKey(s_Window, key);
            sKeyState keyState;
            keyState.Pressed = state == GLFW_PRESS;
            keyState.Released = state == GLFW_RELEASE;
            keyState.Hold = state == GLFW_REPEAT;
            return keyState;
        }

        bool MInput::MousePressed(const eMouse mouse)
        {
            return glfwGetMouseButton(s_Window, mouse) == GLFW_PRESS;
        }

        bool MInput::MouseReleased(const eMouse mouse)
        {
            return glfwGetMouseButton(s_Window, mouse) == GLFW_RELEASE;
        }

        bool MInput::MouseHold(const eMouse mouse)
        {
            return glfwGetMouseButton(s_Window, mouse) == GLFW_REPEAT;
        }

        sMouseState MInput::GetMouseState(const eMouse mouse)
        {
            int state = glfwGetMouseButton(s_Window, mouse);
            sMouseState mouseState;
            mouseState.Pressed = state == GLFW_PRESS;
            mouseState.Released = state == GLFW_RELEASE;
            mouseState.Hold = state == GLFW_REPEAT;
            return mouseState;
        }
    }
}
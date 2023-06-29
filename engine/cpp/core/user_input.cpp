#include <glfw3.h>

#include <core/user_input.hpp>

namespace xpe {

    namespace core {

        static GLFWwindow* s_Window = nullptr;

        void cUserInputManager::Init(void* window) {
            s_Window = static_cast<GLFWwindow*>(window);
            m_WindowEvents.Reserve(5);
            m_KeyEvents.Reserve(5);
            m_MouseEvents.Reserve(5);

            glfwSetWindowUserPointer(s_Window, this);
            InitWindowCallbacks();
            InitKeyCallbacks();
            InitMouseCallbacks();
            InitCursorCallbacks();
        }

        void cUserInputManager::InitWindowCallbacks() {
            glfwSetWindowCloseCallback(s_Window, [](GLFWwindow* window) {
                auto* ui = static_cast<cUserInputManager*>(glfwGetWindowUserPointer(window));
                for (const auto& event : ui->GetWindowEvents().GetEvents()) {
                    event.Listener->WindowClosed();
                }
            });

            glfwSetWindowPosCallback(s_Window, [](GLFWwindow* window, int x, int y) {
                auto* ui = static_cast<cUserInputManager*>(glfwGetWindowUserPointer(window));
                for (const auto& event : ui->GetWindowEvents().GetEvents()) {
                    event.Listener->WindowMoved(x, y);
                }
            });

            glfwSetWindowSizeCallback(s_Window, [](GLFWwindow* window, int w, int h) {
                auto* ui = static_cast<cUserInputManager*>(glfwGetWindowUserPointer(window));
                for (const auto& event : ui->GetWindowEvents().GetEvents()) {
                    event.Listener->WindowResized(w, h);
                }
            });

            glfwSetFramebufferSizeCallback(s_Window, [](GLFWwindow* window, int w, int h) {
                auto* ui = static_cast<cUserInputManager*>(glfwGetWindowUserPointer(window));
                for (const auto& event : ui->GetWindowEvents().GetEvents()) {
                    event.Listener->WindowFrameResized(w, h);
                }
            });

            glfwSetWindowFocusCallback(s_Window, [](GLFWwindow* window, int focused) {
                auto* ui = static_cast<cUserInputManager*>(glfwGetWindowUserPointer(window));
                if (focused == GLFW_TRUE) {
                    for (const auto& event : ui->GetWindowEvents().GetEvents()) {
                        event.Listener->WindowFocused();
                    }
                } else {
                    for (const auto& event : ui->GetWindowEvents().GetEvents()) {
                        event.Listener->WindowFocusLost();
                    }
                }
            });
        }

        void cUserInputManager::InitKeyCallbacks() {
            glfwSetKeyCallback(s_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                auto* ui = static_cast<cUserInputManager*>(glfwGetWindowUserPointer(window));
                switch (action) {

                    case GLFW_PRESS:
                        for (const auto& event : ui->GetKeyEvents().GetEvents()) {
                            event.Listener->KeyPressed(static_cast<eKey>(key));
                        }
                        break;

                    case GLFW_RELEASE:
                        for (const auto& event : ui->GetKeyEvents().GetEvents()) {
                            event.Listener->KeyReleased(static_cast<eKey>(key));
                        }
                        break;

                    case GLFW_REPEAT:
                        for (const auto& event : ui->GetKeyEvents().GetEvents()) {
                            event.Listener->KeyHold(static_cast<eKey>(key));
                        }
                        break;

                }
            });
        }

        void cUserInputManager::InitMouseCallbacks() {
            glfwSetMouseButtonCallback(s_Window, [](GLFWwindow* window, int button, int action, int mods) {
                auto* ui = static_cast<cUserInputManager*>(glfwGetWindowUserPointer(window));
                switch (action) {

                    case GLFW_PRESS:
                        for (const auto& event : ui->GetMouseEvents().GetEvents()) {
                            event.Listener->MousePressed(static_cast<eMouse>(button));
                        }
                        break;

                    case GLFW_RELEASE:
                        for (const auto& event : ui->GetMouseEvents().GetEvents()) {
                            event.Listener->MouseReleased(static_cast<eMouse>(button));
                        }
                        break;

                    case GLFW_REPEAT:
                        for (const auto& event : ui->GetMouseEvents().GetEvents()) {
                            event.Listener->MouseHold(static_cast<eMouse>(button));
                        }
                        break;

                }
            });

            glfwSetScrollCallback(s_Window, [](GLFWwindow* window, double x, double y) {
                auto* ui = static_cast<cUserInputManager*>(glfwGetWindowUserPointer(window));
                for (const auto& event : ui->GetMouseEvents().GetEvents()) {
                    event.Listener->MouseScrollChanged(x, y);
                }
            });
        }

        void cUserInputManager::InitCursorCallbacks() {
            glfwSetCursorPosCallback(s_Window, [](GLFWwindow* window, double x, double y) {
                auto* ui = static_cast<cUserInputManager*>(glfwGetWindowUserPointer(window));
                for (const auto& event : ui->GetCursorEvents().GetEvents()) {
                    event.Listener->CursorMoved(x, y);
                }
            });

            glfwSetCursorEnterCallback(s_Window, [](GLFWwindow* window, int entered) {
                auto* ui = static_cast<cUserInputManager*>(glfwGetWindowUserPointer(window));
                if (entered == GLFW_TRUE) {
                    for (const auto& event : ui->GetCursorEvents().GetEvents()) {
                        event.Listener->CursorEntered();
                    }
                } else {
                    for (const auto& event : ui->GetCursorEvents().GetEvents()) {
                        event.Listener->CursorLeft();
                    }
                }
            });
        }

        void cUserInputManager::Free() {
            m_WindowEvents.Clear();
            m_KeyEvents.Clear();
            m_MouseEvents.Clear();
        }

        void cUserInputManager::CaptureCursor() {
            double xPos = 0.0;
            double yPos = 0.0;
            glfwGetCursorPos(s_Window, &xPos, &yPos);

            glm::vec2 cursorPosition = glm::vec2(xPos, yPos);
            m_Cursor.Delta = cursorPosition - m_Cursor.Position;
            m_Cursor.Position = cursorPosition;
        }

        bool cUserInputManager::isKeyPressed(const eKey key) {
            return glfwGetKey(s_Window, key) == GLFW_PRESS;
        }

        bool cUserInputManager::isKeyReleased(const eKey key) {
            return glfwGetKey(s_Window, key) == GLFW_RELEASE;
        }

        bool cUserInputManager::isKeyHold(const eKey key) {
            return glfwGetKey(s_Window, key) == GLFW_REPEAT;
        }

        KeyState cUserInputManager::GetKeyState(const eKey key) {
            int state = glfwGetKey(s_Window, key);
            KeyState keyState;
            keyState.Pressed = state == GLFW_PRESS;
            keyState.Released = state == GLFW_RELEASE;
            keyState.Hold = state == GLFW_REPEAT;
            return keyState;
        }

        bool cUserInputManager::isMousePressed(const eMouse mouse) {
            return glfwGetMouseButton(s_Window, mouse) == GLFW_PRESS;
        }

        bool cUserInputManager::isMouseReleased(const eMouse mouse) {
            return glfwGetMouseButton(s_Window, mouse) == GLFW_RELEASE;
        }

        bool cUserInputManager::isMouseHold(const eMouse mouse) {
            return glfwGetMouseButton(s_Window, mouse) == GLFW_REPEAT;
        }

        MouseState cUserInputManager::GetMouseState(const eMouse mouse) {
            int state = glfwGetMouseButton(s_Window, mouse);
            MouseState mouseState;
            mouseState.Pressed = state == GLFW_PRESS;
            mouseState.Released = state == GLFW_RELEASE;
            mouseState.Hold = state == GLFW_REPEAT;
            return mouseState;
        }

        void cUserInputManager::AddWindowEventListener(WindowEventListener* listener, int priority) {
            m_WindowEvents.AddEvent(listener, priority);
        }

        void cUserInputManager::AddKeyEventListener(KeyEventListener* listener, int priority) {
            m_KeyEvents.AddEvent(listener, priority);
        }

        void cUserInputManager::AddMouseEventListener(MouseEventListener* listener, int priority) {
            m_MouseEvents.AddEvent(listener, priority);
        }

        void cUserInputManager::AddCursorEventListener(CursorEventListener* listener, int priority) {
            m_CursorEvents.AddEvent(listener, priority);
        }

        void cUserInputManager::RemoveWindowEventListener(WindowEventListener* listener) {
            m_WindowEvents.RemoveEvent(listener);
        }

        void cUserInputManager::RemoveKeyEventListener(KeyEventListener* listener) {
            m_KeyEvents.RemoveEvent(listener);
        }

        void cUserInputManager::RemoveMouseEventListener(MouseEventListener* listener) {
            m_MouseEvents.RemoveEvent(listener);
        }

        void cUserInputManager::RemoveCursorEventListener(CursorEventListener* listener) {
            m_CursorEvents.RemoveEvent(listener);
        }

    }

}
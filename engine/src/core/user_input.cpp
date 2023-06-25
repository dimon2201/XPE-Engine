#include <glfw3.h>
#include <memory>

#include "user_input.hpp"
#include "windowing.hpp"

void xpe::core::_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    cUserInputManager* ui = (cUserInputManager*)glfwGetWindowUserPointer(window);
    
    if (action == GLFW_RELEASE)
    {
        ui->_keys[key] = eKeyState::RELEASED;
    }
    else if (action == GLFW_PRESS)
    {
        ui->_keys[key] = eKeyState::PRESSED;
    }
    else
    {
        ui->_keys[key] = eKeyState::NONE;
    }
}

void xpe::core::_MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    cUserInputManager* ui = (cUserInputManager*)glfwGetWindowUserPointer(window);

    eMouseButton mb;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        mb = eMouseButton::LEFT;
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        mb = eMouseButton::MIDDLE;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        mb = eMouseButton::RIGHT;
    }

    if (action == GLFW_RELEASE)
    {
        ui->_mb[(usize)mb] = eKeyState::RELEASED;
    }
    else if (action == GLFW_PRESS)
    {
        ui->_mb[(usize)mb] = eKeyState::PRESSED;
    }
    else
    {
        ui->_mb[(usize)mb] = eKeyState::NONE;
    }
}

xpe::core::cUserInputManager::cUserInputManager()
{
    memset(&_keys[0], 0, sizeof(eKeyState) * 256);
}

xpe::core::cUserInputManager::~cUserInputManager()
{
}

void xpe::core::cUserInputManager::Init(Window* window)
{
    GLFWwindow* instance = (GLFWwindow*)window->GetInstance();

    glfwSetKeyCallback(instance, &_KeyCallback);
    glfwSetMouseButtonCallback(instance, &_MouseButtonCallback);
}

void xpe::core::cUserInputManager::CaptureCursor(GLFWwindow* window)
{
    double xPos = 0.0;
    double yPos = 0.0;
    glfwGetCursorPos(window, &xPos, &yPos);

    glm::vec2 cursorPosition = glm::vec2(xPos, yPos);
    _cursorPositionDelta = cursorPosition - _cursorPosition;
    _cursorPosition = cursorPosition;
}
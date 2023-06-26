#include <gtc/quaternion.hpp>
#include <gtc/matrix_transform.hpp>

#include "viewer.hpp"
#include "../core/ecs.hpp"
#include "../core/user_input.hpp"

void xpe::viewer::ViewerUpdate(core::f32 time, core::cUserInputManager* ui, core::cTransformComponent* transform, core::cViewerComponent* viewer)
{
    const core::f32 moveSpeed = 10.0f * time;
    const core::f32 lookSpeed = 50.0f * time;

    if (ui->GetKeyPressed('W')) { viewer->_Keys[0] = moveSpeed; }
    if (ui->GetKeyPressed('A')) { viewer->_Keys[1] = moveSpeed; }
    if (ui->GetKeyPressed('S')) { viewer->_Keys[2] = moveSpeed; }
    if (ui->GetKeyPressed('D')) { viewer->_Keys[3] = moveSpeed; }
    if (ui->GetKeyReleased('W')) { viewer->_Keys[0] = 0.0f; }
    if (ui->GetKeyReleased('A')) { viewer->_Keys[1] = 0.0f; }
    if (ui->GetKeyReleased('S')) { viewer->_Keys[2] = 0.0f; }
    if (ui->GetKeyReleased('D')) { viewer->_Keys[3] = 0.0f; }
    if (ui->GetMouseButtonPressed(core::eMouseButton::LEFT)) { viewer->_LMBFlag = 1; }
    if (ui->GetMouseButtonReleased(core::eMouseButton::LEFT)) { viewer->_LMBFlag = 0; }

    if (viewer->_LMBFlag)
    {
        glm::vec2 delta = ui->GetMouseCursorPositionDelta();

        transform->Rotation.y += delta.x * lookSpeed;
        transform->Rotation.x += delta.y * lookSpeed;
    }

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::quat qx = glm::angleAxis(glm::radians(transform->Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat qy = glm::angleAxis(glm::radians(transform->Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat qz = glm::angleAxis(glm::radians(transform->Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec3 direction = qz * qy * qx * glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 right = glm::normalize(glm::cross(direction, up));

    transform->Position += direction * viewer->_Keys[0];
    transform->Position += right * viewer->_Keys[1];
    transform->Position -= direction * viewer->_Keys[2];
    transform->Position -= right * viewer->_Keys[3];
    
    viewer->ViewProjection =
        glm::perspectiveFovLH(glm::radians(65.0f), 800.0f, 600.0f, 0.01f, 100.0f) *
        glm::lookAtLH(transform->Position, transform->Position + direction, up);
}
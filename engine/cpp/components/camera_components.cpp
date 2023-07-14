#include <components/camera_components.h>

namespace xpe {

    namespace ecs {

        glm::vec3 CameraComponent::GetUpDirection() const {
            return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        glm::vec3 CameraComponent::GetRightDirection() const {
            return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
        }

        glm::vec3 CameraComponent::GetForwardDirection() const {
            return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
        }

        glm::vec3 CameraComponent::CalculatePosition() const {
            return FocalPoint - GetForwardDirection() * Distance;
        }

        glm::quat CameraComponent::GetOrientation() const {
            return glm::vec3(-Pitch, -Yaw, -Roll);
        }

    }

}
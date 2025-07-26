#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace CPL 
{
    glm::mat4 Camera::GetViewMatrix() const {
        return glm::lookAt(Position, Target, Up);
    }

    glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const {
        return glm::perspective(glm::radians(FOV), aspectRatio, NearClip, FarClip);
    }
}

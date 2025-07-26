#pragma once
#include <glm/glm.hpp>

namespace CPL {
    class Camera {
    public:
        glm::vec3 Position = { 0.0f, 0.0f, 3.0f };
        glm::vec3 Target = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Up = { 0.0f, 1.0f, 0.0f };

        float FOV = 45.0f;
        float NearClip = 0.1f;
        float FarClip = 100.0f;

        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix(float aspectRatio) const;
    };
}

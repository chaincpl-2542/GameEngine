#include "CameraController.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace CPL {
    CameraController::CameraController(Camera& cam, float aspect)
        : camera(cam), isRightMouseDown(false) 
    {
        camera.Position = glm::vec3(0, 0, 3);
        camera.Target = camera.Position + front;
    }

    void CameraController::OnUpdate(float dt) 
    {
        if (!isRightMouseDown) return;

        glm::vec3 right = glm::normalize(glm::cross(front, camera.Up));
        glm::vec3 up = glm::normalize(glm::cross(right, front));

        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
            camera.Position += front * speed * dt;
        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS)
            camera.Position -= front * speed * dt;
        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS)
            camera.Position -= right * speed * dt;
        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS)
            camera.Position += right * speed * dt;

        camera.Target = camera.Position + front;
    }

    void CameraController::OnMouseMove(double xpos, double ypos) 
    {
        if (!isRightMouseDown) return;

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        pitch = glm::clamp(pitch, -89.0f, 89.0f);

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(direction);
    }

    void CameraController::SetRightMouseDown(bool down) 
    {
        isRightMouseDown = down;
        firstMouse = true;
    }
}

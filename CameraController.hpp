#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Camera.hpp"

namespace CPL 
{
    class CameraController 
    {
    public:
        CameraController(Camera& cam, float aspect);

        void OnUpdate(float deltaTime);
        void OnMouseMove(double xpos, double ypos);
        void OnScroll(double yoffset);

        void SetRightMouseDown(bool down);

        void ResetCamera();

    private:
        Camera& camera;
        float speed = 3.0f;
        float sensitivity = 0.1f;

        bool isRightMouseDown = false;
        bool firstMouse = true;
        float yaw = -90.0f, pitch = 0.0f;
        double lastX = 400, lastY = 300;

        glm::vec3 front = { 0, 0, -1 };

    };
}

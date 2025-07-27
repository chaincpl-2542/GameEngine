#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include <iostream>

#include "Shader.hpp"
#include "TextureLoader.hpp"
#include "ImGuiLayer.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "CameraController.hpp"
#include "miniaudio.h"
#include "SoundManager.hpp"

#define MINIAUDIO_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

using namespace std;
using namespace CPL;
using namespace glm;

const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

enum class ShapeType { Triangle = 1, Circle, Quad, Cube, Pyramid };

Mesh* GetMesh(ShapeType shape) {
    static Mesh triangle = Mesh::CreateTriangle();
    static Mesh circle = Mesh::CreateCircle();
    static Mesh quad = Mesh::CreateQuad();
    static Mesh cube = Mesh::CreateCube();
    static Mesh pyramid = Mesh::CreatePyramid();

    switch (shape) {
    case ShapeType::Triangle: return &triangle;
    case ShapeType::Circle: return &circle;
    case ShapeType::Quad: return &quad;
    case ShapeType::Cube: return &cube;
    case ShapeType::Pyramid: return &pyramid;
    }
    return &triangle;
}

CameraController* g_camController = nullptr;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse && g_camController) {
        g_camController->OnMouseMove(xpos, ypos);
    }

    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && g_camController)
        {
            bool isDown = (action == GLFW_PRESS);
            g_camController->SetRightMouseDown(isDown);

            glfwSetInputMode(window, GLFW_CURSOR,
                isDown ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
    }

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

// ───────────────────────────────────────────
int main() {
    //Sound
    SoundManager::Get().Init();
    SoundManager::Get().PlaySound("assets/Sound.mp3");
    /////

    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Mesh Viewer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    SetupImGui(window);

    Shader shader("assets/vertex.glsl", "assets/fragment.glsl");
    unsigned int texture = LoadTexture("assets/brick.png");

    ShapeType currentShape = ShapeType::Triangle;
    vec3 position(0.0f), rotation(0.0f);
    float size = 1.0f;
	float ambient_intensity = 1.0f;

    Camera camera;
    CameraController camController(camera, (float)WINDOW_WIDTH / WINDOW_HEIGHT);
    g_camController = &camController;

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    float lastFrame = 0.0f;
    static glm::vec3 lightPos = glm::vec3(3, 3, 3);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) currentShape = ShapeType::Triangle;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) currentShape = ShapeType::Circle;
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) currentShape = ShapeType::Quad;
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) currentShape = ShapeType::Cube;
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) currentShape = ShapeType::Pyramid;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Inspector");
        ImGui::DragFloat3("Position", &position.x, 0.1f);
        ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);
        ImGui::DragFloat("Size", &size, 0.1f);
        ImGui::DragFloat3("Cam Position", &camera.Position.x, 0.1f);
        ImGui::DragFloat3("Cam Target", &camera.Target.x, 0.1f);
        ImGui::DragFloat3("Light Position", &lightPos.x, 0.1f);
        ImGui::SliderFloat("Ambient Intensity", &ambient_intensity,0.0f, 1.0f);

        ImGui::End();

        shader.use();
        glBindTexture(GL_TEXTURE_2D, texture);

        mat4 model = mat4(1.0f);
        model = translate(model, position);
        model = rotate(model, rotation.x, vec3(1, 0, 0));
        model = rotate(model, rotation.y, vec3(0, 1, 0));
        model = rotate(model, rotation.z, vec3(0, 0, 1));

        mat4 view = camera.GetViewMatrix();
        mat4 projection = camera.GetProjectionMatrix((float)WINDOW_WIDTH / WINDOW_HEIGHT);

        shader.setFloat("size", size);
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setFloat("ambientIntensity", ambient_intensity);
        shader.setInt("mytexture", 0);
        shader.setVec3("lightDir", glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("lightPos", lightPos);

        GetMesh(currentShape)->Draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        camController.OnUpdate(deltaTime);

        glfwSwapBuffers(window);
    }

    ShutdownImGui();
    glfwTerminate();

    SoundManager::Get().Destroy();

    return 0;
}
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <optional>

#include "Shader.hpp"
#include "TextureLoader.hpp"
#include "ImGuiLayer.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "CameraController.hpp"
#include "miniaudio.h"
#include "SoundManager.hpp"
#include "SceneManager.hpp"

#define MINIAUDIO_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

using namespace std;
using namespace CPL;
using namespace glm;
using json = nlohmann::json;

const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

enum class ShapeType { Triangle = 1, Circle, Quad, Cube, Pyramid };

struct SceneObject {
    ShapeType shape;
    glm::vec3 position;
    glm::vec3 rotation;
    float size = 1.0f;
};

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
int main() 
{
    //Test load Json
    json config;
    std::ifstream file("config.json");
    if (!file.is_open()) {
        std::cerr << "Failed to open config.json" << std::endl;
        return -1;
    }
    file >> config;
    ///////

    //Sound
    SoundManager::Get().Init();
    SoundManager::Get().PlaySound(config["assets"]["sound"]);
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

    //Load Shader
    Shader shader(
        config["assets"]["vertexShader"].get<std::string>().c_str(),
        config["assets"]["fragmentShader"].get<std::string>().c_str()
    );
    //////

    //Load Texture
    unsigned int texture = LoadTexture(config["assets"]["texture"]);

    vector<SceneObject> sceneObjects;
    int selectedObjectIndex = -1;
    float ambient_intensity = 1.0f;

    // Test Camera
    Camera camera;

    if (config.contains("camera")) {
        const auto& camConfig = config["camera"];

        camera.Position = glm::vec3(
            config["camera"]["position"][0],
            config["camera"]["position"][1],
            config["camera"]["position"][2]
        );

        camera.FOV = config["camera"]["fov"];
        camera.NearClip = config["camera"]["nearClip"];
        camera.FarClip = config["camera"]["farClip"];

        // Set default forward target
        camera.Target = camera.Position + glm::vec3(0, 0, -1);
    }

    // CameraController
    CameraController camController(camera, (float)WINDOW_WIDTH / WINDOW_HEIGHT);
    g_camController = &camController;

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    float lastFrame = 0.0f;
    static glm::vec3 lightPos = glm::vec3(
        config["light"]["position"][0],
        config["light"]["position"][1],
        config["light"]["position"][2]
    );

    static int selectedShapeIndex = 0;
    const char* shapeNames[] = { "Triangle", "Circle", "Quad", "Cube", "Pyramid" };

    // ImGui
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ───────── Inspector (แทนที่บล็อกเดิมทั้งหมด) ─────────
        ImGui::Begin("Inspector");

        // ── SECTION 1 : สร้าง Shape ────────────────────────────
        static int selectedShapeIndex = 0;
        const char* shapeNames[] = { "Triangle", "Circle", "Quad", "Cube", "Pyramid" };

        ImGui::Separator();
        ImGui::Text("Create Shape");
        ImGui::Combo("##shapeType", &selectedShapeIndex,shapeNames, IM_ARRAYSIZE(shapeNames));
        ImGui::SameLine();
        ImGui::Text("Shape Type");

        if (ImGui::Button("Add Shape")) {
            SceneObject obj;
            obj.shape = static_cast<ShapeType>(selectedShapeIndex + 1); // enum เริ่มที่ 1
            obj.position = {};
            obj.rotation = {};
            obj.size = 1.0f;
            sceneObjects.push_back(obj);
            selectedObjectIndex = (int)sceneObjects.size() - 1;
        }

        // ── SECTION 2 : เลือก & แก้ไข Object ───────────────────
        if (!sceneObjects.empty()) {
            //---- dropdown รายชื่อ object
            std::vector<std::string> labels;
            for (int i = 0; i < sceneObjects.size(); ++i)
                labels.push_back("Object " + std::to_string(i + 1));

            std::vector<const char*> names;
            for (auto& s : labels) names.push_back(s.c_str());

            ImGui::Combo("Select Obj", &selectedObjectIndex,
                names.data(), (int)names.size());

            //---- ปุ่มลบ object ที่เลือก
            if (ImGui::Button("Delete Selected") &&
                selectedObjectIndex >= 0 &&
                selectedObjectIndex < sceneObjects.size()) {
                sceneObjects.erase(sceneObjects.begin() + selectedObjectIndex);
                selectedObjectIndex = -1;
            }

            //---- แก้ไข transform ของ object ที่เลือก
            if (selectedObjectIndex >= 0 &&
                selectedObjectIndex < sceneObjects.size()) {
                SceneObject& sel = sceneObjects[selectedObjectIndex];

                ImGui::DragFloat3("Position", &sel.position.x, 0.1f);
                ImGui::DragFloat3("Rotation", &sel.rotation.x, 0.1f);
                ImGui::DragFloat("Size", &sel.size, 0.1f);
            }
        }

        // ── SECTION 3 : Global (Camera/Light/Ambient) ──────────
        ImGui::Separator();
        ImGui::DragFloat3("Cam Position", &camera.Position.x, 0.1f);
        ImGui::DragFloat3("Cam Target", &camera.Target.x, 0.1f);
        ImGui::DragFloat3("Light Pos", &lightPos.x, 0.1f);
        ImGui::SliderFloat("Ambient", &ambient_intensity, 0.0f, 1.0f);

        // ── SECTION 4 : Save / Load Scene ──────────────────────
        static char sceneName[128] = "scene1.json";
        ImGui::InputText("Scene File", sceneName, IM_ARRAYSIZE(sceneName));

        if (ImGui::Button("Save Scene")) {
            json scene;

            //-- เซฟ objects
            for (const auto& o : sceneObjects) {
                json j;                        // object node
                j["position"] = { o.position.x, o.position.y, o.position.z };
                j["rotation"] = { o.rotation.x, o.rotation.y, o.rotation.z };
                j["size"] = o.size;
                j["shape"] = (int)o.shape;
                scene["objects"].push_back(j);
            }
            //-- เซฟ global
            scene["light"] = { lightPos.x, lightPos.y, lightPos.z };
            scene["ambient"] = ambient_intensity;
            scene["camera"]["position"] = { camera.Position.x, camera.Position.y, camera.Position.z };
            scene["camera"]["target"] = { camera.Target.x,   camera.Target.y,   camera.Target.z };

            std::ofstream out(sceneName);
            if (out) { out << scene.dump(4); }
        }

        ImGui::SameLine();

        if (ImGui::Button("Load Scene")) {
            std::ifstream in(sceneName);
            if (in) {
                json scene; in >> scene;

                //-- โหลด objects
                sceneObjects.clear();
                for (auto& j : scene["objects"]) {
                    SceneObject o;
                    o.position = { j["position"][0], j["position"][1], j["position"][2] };
                    o.rotation = { j["rotation"][0], j["rotation"][1], j["rotation"][2] };
                    o.size = j["size"];
                    o.shape = (ShapeType)j["shape"];
                    sceneObjects.push_back(o);
                }

                //-- โหลด global
                lightPos = { scene["light"][0], scene["light"][1], scene["light"][2] };
                ambient_intensity = scene["ambient"];
                camera.Position = { scene["camera"]["position"][0],
                                    scene["camera"]["position"][1],
                                    scene["camera"]["position"][2] };
                camera.Target = { scene["camera"]["target"][0],
                                    scene["camera"]["target"][1],
                                    scene["camera"]["target"][2] };
                selectedObjectIndex = -1;
            }
        }

        ImGui::End();


        shader.use();
        glBindTexture(GL_TEXTURE_2D, texture);

        mat4 view = camera.GetViewMatrix();
        mat4 projection = camera.GetProjectionMatrix((float)WINDOW_WIDTH / WINDOW_HEIGHT);

        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setFloat("ambientIntensity", ambient_intensity);
        shader.setInt("mytexture", 0);
        shader.setVec3("lightDir", { 0,0,1 });
        shader.setVec3("lightColor", { 1,1,1 });
        shader.setVec3("lightPos", lightPos);

        for (const auto& obj : sceneObjects)
        {
            mat4 model = mat4(1.0f);
            model = translate(model, obj.position);
            model = rotate(model, obj.rotation.x, { 1,0,0 });
            model = rotate(model, obj.rotation.y, { 0,1,0 });
            model = rotate(model, obj.rotation.z, { 0,0,1 });
            shader.setMat4("model", model);
            shader.setFloat("size", obj.size);
            GetMesh(obj.shape)->Draw();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        for (const auto& obj : sceneObjects) {
            mat4 model = mat4(1.0f);
            model = translate(model, obj.position);
            model = rotate(model, obj.rotation.x, vec3(1, 0, 0));
            model = rotate(model, obj.rotation.y, vec3(0, 1, 0));
            model = rotate(model, obj.rotation.z, vec3(0, 0, 1));

            shader.setFloat("size", obj.size);
            shader.setMat4("model", model);

            GetMesh(obj.shape)->Draw();
        }

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
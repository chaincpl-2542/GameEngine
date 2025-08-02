#include "SceneManager.hpp"
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

bool SceneManager::SaveScene(const std::string& path, const SceneData& data) {
    json j;
    j["position"] = { data.position.x, data.position.y, data.position.z };
    j["rotation"] = { data.rotation.x, data.rotation.y, data.rotation.z };
    j["size"] = data.size;
    j["light"] = { data.lightPos.x, data.lightPos.y, data.lightPos.z };
    j["shape"] = data.shape.has_value() ? static_cast<int>(data.shape.value()) : -1;

    std::ofstream out(path);
    if (!out.is_open()) return false;

    out << j.dump(4);
    return true;
}

bool SceneManager::LoadScene(const std::string& path, SceneData& data) {
    std::ifstream in(path);
    if (!in.is_open()) return false;

    json j;
    in >> j;

    data.position = glm::vec3(j["position"][0], j["position"][1], j["position"][2]);
    data.rotation = glm::vec3(j["rotation"][0], j["rotation"][1], j["rotation"][2]);
    data.size = j["size"];
    data.lightPos = glm::vec3(j["light"][0], j["light"][1], j["light"][2]);

    int shapeId = j["shape"];
    if (shapeId != -1) {
        data.shape = static_cast<ShapeType>(shapeId);
    }
    else {
        data.shape.reset();
    }

    return true;
}

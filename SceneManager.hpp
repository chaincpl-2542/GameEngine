#pragma once
#include <string>
#include <glm/glm.hpp>
#include <optional>

enum class ShapeType;

struct SceneData {
    std::optional<ShapeType> shape;
    glm::vec3 position = { 0, 0, 0 };
    glm::vec3 rotation = { 0, 0, 0 };
    float size = 1.0f;
    glm::vec3 lightPos = { 3, 3, 3 };
};

class SceneManager {
public:
    static bool SaveScene(const std::string& path, const SceneData& data);
    static bool LoadScene(const std::string& path, SceneData& data);
};

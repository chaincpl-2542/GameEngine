#pragma once
#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Skybox {
public:
    bool   Load(const std::vector<std::string>& faces);  //VAO-VBO 
    void   Draw(const glm::mat4& view, const glm::mat4& proj);
    void   Destroy();
private:
    GLuint vao = 0, vbo = 0, cubemap = 0, shader = 0;
};

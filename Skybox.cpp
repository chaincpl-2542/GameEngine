#include "Skybox.hpp"
#include "stb_image.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// vertex + fragment shader
static const char* vSrc = R"(#version 330 core
layout(location=0) in vec3 pos;
out vec3 TexDir;
uniform mat4 VP;
void main() {
    TexDir = pos;
    gl_Position = VP * vec4(pos,1.0);
})";
static const char* fSrc = R"(#version 330 core
in vec3 TexDir;
out vec4 Frag;
uniform samplerCube sky;
void main() { Frag = texture(sky, TexDir); })";

static GLuint compile(GLenum type, const char* src) {
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr); glCompileShader(id);
    int ok; glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetShaderInfoLog(id, 512, nullptr, log);
        std::cerr << "shader err\n" << log << std::endl;
    }
    return id;
}
static GLuint linkProg() {
    GLuint vs = compile(GL_VERTEX_SHADER, vSrc);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fSrc);
    GLuint pr = glCreateProgram();
    glAttachShader(pr, vs); glAttachShader(pr, fs); glLinkProgram(pr);
    glDeleteShader(vs); glDeleteShader(fs);
    return pr;
}

bool Skybox::Load(const std::vector<std::string>& faces)
{
    // cube vertices
    float verts[] = {
        -1,-1,-1,  1,-1,-1,  1, 1,-1,  1, 1,-1, -1, 1,-1, -1,-1,-1,
        -1,-1, 1,  1,-1, 1,  1, 1, 1,  1, 1, 1, -1, 1, 1, -1,-1, 1,
        -1, 1, 1, -1, 1,-1, -1,-1,-1, -1,-1,-1, -1,-1, 1, -1, 1, 1,
         1, 1, 1,  1, 1,-1,  1,-1,-1,  1,-1,-1,  1,-1, 1,  1, 1, 1,
        -1,-1,-1,  1,-1,-1,  1,-1, 1,  1,-1, 1, -1,-1, 1, -1,-1,-1,
        -1, 1,-1,  1, 1,-1,  1, 1, 1,  1, 1, 1, -1, 1, 1, -1, 1,-1
    };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // cubemap texture
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    int w, h, chan;
    for (int i = 0; i < 6; ++i) {
        unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &chan, 0);
        if (data)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, chan == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    shader = linkProg();
    return true;
}
void Skybox::Draw(const glm::mat4& view, const glm::mat4& proj)
{
    glDepthFunc(GL_LEQUAL);
    glUseProgram(shader);

    glm::mat4 V = glm::mat4(glm::mat3(view));
    glm::mat4 VP = proj * V;
    glUniformMatrix4fv(glGetUniformLocation(shader, "VP"), 1, GL_FALSE, glm::value_ptr(VP));
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    glUniform1i(glGetUniformLocation(shader, "sky"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
}
void Skybox::Destroy() {
    glDeleteProgram(shader);
    glDeleteTextures(1, &cubemap);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

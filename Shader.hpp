#pragma once
#include <string>
#include <glm/glm.hpp>

namespace CPL
{
    class Shader {
    public:
        unsigned int ID;

        Shader(const char* vertexPath, const char* fragmentPath);
        ~Shader();

        void use() const;
        void setFloat(const std::string& name, float value) const;
        void setInt(const std::string& name, int value) const;
        void setMat4(const std::string& name, const glm::mat4& mat) const;

        void setVec3(const std::string& name, const glm::vec3& value) const;

    private:
        unsigned int compileShader(unsigned int type, const char* source);
        std::string loadFile(const char* path);
    };
}
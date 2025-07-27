#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace CPL
{
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Color;
        glm::vec2 TexCoord;
        glm::vec3 Normal;
        Vertex(
            glm::vec3 pos,
            glm::vec3 color = glm::vec3(1, 1, 1),
            glm::vec2 texCoord = glm::vec2(0, 0),
            glm::vec3 normal = glm::vec3(0, 0, 1)
        )
            : Position(pos), Color(color), TexCoord(texCoord), Normal(normal) {
        }
    };

    class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        ~Mesh();

        void Draw() const;

        static Mesh CreateTriangle();

        static Mesh CreateQuad(float width = 1.0f, float height = 1.0f);

        static Mesh CreateCircle(float radius = 1.0f, int segments = 32);

        static Mesh CreateCube(float size = 1.0f);

        static Mesh CreatePyramid(float base = 1.0f, float height = 1.0f);


    private:
        unsigned int VAO, VBO, EBO;
        void setupMesh();
    };
}

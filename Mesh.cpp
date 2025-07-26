// Mesh.cpp
#include "Mesh.hpp"

namespace CPL
{
    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
        : vertices(vertices), indices(indices)
    {
        setupMesh();
    }

    Mesh::~Mesh()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void Mesh::setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
        glEnableVertexAttribArray(0);

        // Color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
        glEnableVertexAttribArray(1);

        // TexCoord
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void Mesh::Draw() const
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    Mesh Mesh::CreateTriangle()
    {
        std::vector<Vertex> verts = {
            Vertex({ 0.0f,  0.5f, 0.0f }, {1,0,0}, {0.5f, 1.0f}),
            Vertex({ 0.5f, -0.5f, 0.0f }, {0,1,0}, {1.0f, 0.0f}),
            Vertex({-0.5f, -0.5f, 0.0f }, {0,0,1}, {0.0f, 0.0f}),
        };

        std::vector<unsigned int> inds = {
            0, 1, 2
        };

        return Mesh(verts, inds);
    }

    Mesh Mesh::CreateQuad(float width, float height)
    {
        float hw = width * 0.5f;
        float hh = height * 0.5f;

        std::vector<Vertex> verts = {
            Vertex({ hw,  hh, 0.0f }, {1,0,0}, {1,1}),  // top-right
            Vertex({ hw, -hh, 0.0f }, {0,1,0}, {1,0}),  // bottom-right
            Vertex({-hw, -hh, 0.0f }, {0,0,1}, {0,0}),  // bottom-left
            Vertex({-hw,  hh, 0.0f }, {1,1,0}, {0,1})   // top-left
        };

        std::vector<unsigned int> inds = {
            0, 1, 3,
            1, 2, 3
        };

        return Mesh(verts, inds);
    }

    Mesh Mesh::CreateCircle(float radius, int segments)
    {
        std::vector<Vertex> verts;
        std::vector<unsigned int> inds;

        verts.push_back(Vertex({ 0.0f, 0.0f, 0.0f }, { 1, 0, 0 }, { 0.5f, 0.5f }));

        for (int i = 0; i <= segments; ++i)
        {
            float angle = 2.0f * 3.1415926f * i / segments;
            float x = radius * cos(angle);
            float y = radius * sin(angle);

            float u = (x / (radius * 2.0f)) + 0.5f;
            float v = (y / (radius * 2.0f)) + 0.5f;

            verts.push_back(Vertex({ x, y, 0.0f }, {1, 1, 0 }, { u, v }));
        }

        for (int i = 1; i <= segments; ++i)
        {
            inds.push_back(0);
            inds.push_back(i);
            inds.push_back(i + 1);
        }

        return Mesh(verts, inds);
    }

    Mesh Mesh::CreateCube(float size)
    {
        float h = size * 0.5f;

        std::vector<Vertex> verts = {
            // Front face
            {{-h, -h,  h}, {1, 0, 0}, {0, 0}},
            {{ h, -h,  h}, {0, 1, 0}, {1, 0}},
            {{ h,  h,  h}, {0, 0, 1}, {1, 1}},
            {{-h,  h,  h}, {1, 1, 0}, {0, 1}},

            // Back face
            {{-h, -h, -h}, {1, 0, 1}, {1, 0}},
            {{ h, -h, -h}, {0, 1, 1}, {0, 0}},
            {{ h,  h, -h}, {1, 1, 1}, {0, 1}},
            {{-h,  h, -h}, {0.5, 0.5, 0.5}, {1, 1}},
        };

        std::vector<unsigned int> inds = {
            // Front
            0, 1, 2,  2, 3, 0,
            // Right
            1, 5, 6,  6, 2, 1,
            // Back
            5, 4, 7,  7, 6, 5,
            // Left
            4, 0, 3,  3, 7, 4,
            // Top
            3, 2, 6,  6, 7, 3,
            // Bottom
            4, 5, 1,  1, 0, 4,
        };

        return Mesh(verts, inds);
    }

    Mesh Mesh::CreatePyramid(float base, float height)
    {
        float h = base * 0.5f;

        std::vector<Vertex> verts = {
            // Base (square)
            {{-h, 0.0f, -h}, {1, 0, 0}, {0, 0}},
            {{ h, 0.0f, -h}, {0, 1, 0}, {1, 0}},
            {{ h, 0.0f,  h}, {0, 0, 1}, {1, 1}},
            {{-h, 0.0f,  h}, {1, 1, 0}, {0, 1}},

            // Tip
            {{0.0f, height, 0.0f}, {1, 1, 1}, {0.5f, 0.5f}},
        };

        std::vector<unsigned int> inds = {
            // Base
            0, 1, 2,
            2, 3, 0,

            // Sides (triangles)
            0, 1, 4,
            1, 2, 4,
            2, 3, 4,
            3, 0, 4,
        };

        return Mesh(verts, inds);
    }

}
#version 330 core

layout(location = 0) in vec3 aPos;      // Vertex position
layout(location = 1) in vec3 aColor;    // Vertex color
layout(location = 2) in vec2 aTexCoord; // Texture coordinates

out vec3 vertexColor;
out vec2 texCoord;

uniform float size;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos * size, 1.0);
    vertexColor = aColor;
    texCoord = aTexCoord;
}

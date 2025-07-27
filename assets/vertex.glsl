#version 330 core

layout(location = 0) in vec3 aPos;      // Vertex position
layout(location = 1) in vec3 aColor;    // Vertex color
layout(location = 2) in vec2 aTexCoord; // Texture coordinates
layout(location = 3) in vec3 aNormal;

out vec3 vertexColor;
out vec2 texCoord;
out vec3 FragNormal;
out vec3 FragPos;

uniform float size;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragNormal = normalize(mat3(transpose(inverse(model))) * aNormal);
    gl_Position = projection * view * model * vec4(aPos * size, 1.0);
    FragPos = vec3(gl_Position);
    vertexColor = aColor;
    texCoord = aTexCoord;
}

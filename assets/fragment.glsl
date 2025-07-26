#version 330 core

in vec3 vertexColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D mytexture;

void main() 
{
    FragColor = texture(mytexture, texCoord); // ใช้ texture
}
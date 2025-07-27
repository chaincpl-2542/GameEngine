#version 330 core

in vec3 vertexColor;
in vec2 texCoord;
in vec3 FragNormal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 lightDir;
uniform float ambientIntensity;
uniform sampler2D mytexture;

void main()
{
    vec3 normal = normalize(FragNormal);
    vec3 lightDirection = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDirection), 0.0);
    
    vec3 ambient = lightColor * ambientIntensity;
    vec3 diffuse = lightColor * diff;
    vec3 finalLight = ambient + diffuse;

    FragColor = vec4(finalLight, 1.0) * texture(mytexture, texCoord);
}

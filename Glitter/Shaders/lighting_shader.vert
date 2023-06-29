#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;

out vec3 FragPos;
out vec3 Normal;
out vec3 WorldPos;
out vec2 TexCoords;
out vec3 Tangent;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    FragPos = vec3(modelMatrix * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(modelMatrix))) * normal;
    Tangent = mat3(transpose(inverse(modelMatrix))) * tangent;
    WorldPos = vec3(modelMatrix);
    TexCoords = texCoords;
}

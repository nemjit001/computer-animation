#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 Normal;
out vec3 WorldPos;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    Normal = mat3(transpose(inverse(modelMatrix))) * normal;
    WorldPos = vec3(modelMatrix * vec4(position, 1.0));
}

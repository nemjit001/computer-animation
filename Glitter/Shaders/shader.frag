#version 430

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

out vec4 outColor;

void main()
{
    outColor = vec4(1, 1, 0, 1.0);
}

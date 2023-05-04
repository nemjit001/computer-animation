#version 430

in vec3 fragColor;

void main()
{
    outColor = vec4(fragColor, 1);
}

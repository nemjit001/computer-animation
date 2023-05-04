#version 430

in vec2 vPos;
in vec3 vColor;

out vec3 fragColor;

void main()
{
    gl_Position = vec4(vPos, 0.0, 1.0);
    fragColor = vColor;
}
